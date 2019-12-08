/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : deinterlace.cpp
 * Description : hardware deinterlacing
 * History :
 *
 */

#include <cdx_log.h>

#include <memoryAdapter.h>

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <errno.h>
#define __STDC_FORMAT_MACROS // Otherwise not available in C++.
#include <inttypes.h>

#include "outputCtrl.h"
#include <iniparserapi.h>
#include <cutils/properties.h>
//-----------------------------------------------------------------------------
// relation with deinterlace

#define DI_MODULE_TIMEOUT    0x1055
#define    DI_IOC_MAGIC        'D'

//DI_IOSTART use int instead of pointer* in double 32/64 platform to avoid bad definition.
#define    DI_IOCSTART             _IOWR(DI_IOC_MAGIC, 0, DiRectSizeT)
#define    DI_IOCSTART2            _IOWR(DI_IOC_MAGIC, 1, struct DiRectSizeT)
#define    DI_IOCSETMODE           _IOWR(DI_IOC_MAGIC, 2, struct __di_mode_t)

typedef enum DiPixelformatE2 {
    DI2_FORMAT_NV12      =0x00,
    DI2_FORMAT_NV21      =0x01,
    DI2_FORMAT_MB32_12   =0x02, //UV mapping like NV12
    DI2_FORMAT_MB32_21   =0x03, //UV mapping like NV21
    DI2_FORMAT_YV12 = 0x04, /* 3-plane */
    DI2_FORMAT_YUV422_SP_UVUV = 0x08, /* 2-plane, New in DI_V2.2 */
    DI2_FORMAT_YUV422_SP_VUVU = 0x09, /* 2-plane, New in DI_V2.2 */
    DI2_FORMAT_YUV422P = 0x0c, /* 3-plane, New in DI_V2.2 */
} DiPixelformatE2;

typedef enum DiPixelformatE {
    DI_FORMAT_NV12      =0x00,
    DI_FORMAT_NV21      =0x01,
    DI_FORMAT_MB32_12   =0x02, //UV mapping like NV12
    DI_FORMAT_MB32_21   =0x03, //UV mapping like NV21
} DiPixelformatE;

typedef struct DiRectSizeT {
    unsigned int nWidth;
    unsigned int nHeight;
} DiRectSizeT;

typedef struct DiFbT {
#if (CONF_KERN_BITWIDE == 64)
    unsigned long long         addr[2];  // the address of frame buffer
#else
    uintptr_t         addr[2];  // the address of frame buffer
#endif
    DiRectSizeT       mRectSize;
    DiPixelformatE    eFormat;
} DiFbT;

typedef struct DiFbT2 {
    int               fd;
#if (CONF_KERN_BITWIDE == 64)
    unsigned long long         addr[3];  // the address of frame buffer
#else
    uintptr_t         addr[3];  // the address of frame buffer
#endif
    DiRectSizeT       mRectSize;
    DiPixelformatE2    eFormat;
} DiFbT2;

typedef struct DiParaT {
    DiFbT         mInputFb;           //current frame fb
    DiFbT         mPreFb;          //previous frame fb
    DiRectSizeT   mSourceRegion;   //current frame and previous frame process region
    DiFbT         mOutputFb;       //output frame fb
    DiRectSizeT   mOutRegion;       //output frame region
    unsigned int  nField;          //process field <0-top field ; 1-bottom field>
    /* video infomation <0-is not top_field_first; 1-is top_field_first> */
    unsigned int  bTopFieldFirst;
} DiParaT;

typedef struct DiParaT2 {
    DiFbT2         mInputFb;       /* current frame fb */
    DiFbT2         mPreFb;         /* previous frame fb */
    DiFbT2         mNextFb;        /* next frame fb */

    /* current frame /previous frame and next frame process region */
    DiRectSizeT   mSourceRegion;

    DiFbT2         mOutputFb;      /* output frame fb */
    DiRectSizeT   mOutRegion;     /* output frame region */
    unsigned int  nField;         /* process field <0-first field ; 1-second field> */

    /* video infomation <0-is not top_field_first; 1-is top_field_first> */
    unsigned int  bTopFieldFirst;

    /* unsigned int update_mode; */
    /* update buffer mode <0-update 1 frame, output 2 frames; 1-update 1 frame, output 1 frame> */
} DiParaT2;

enum {
    IOMEM_UNKNOWN,
    IOMEM_CMA,
    IOMEM_IOMMU,
};

struct DiContext
{
    Deinterlace base;

    int fd;
    int picCount;
    DiRectSizeT pVideoSize;
    int iomemType;
};

static int dumpPara(DiParaT *para)
{
    logd("**************************************************************");
    logd("*********************deinterlace info*************************");

    logd(" input_fb: addr=0x%x, 0x%x, format=%d, size=(%d, %d)",
        para->mInputFb.addr[0], para->mInputFb.addr[1], (int)para->mInputFb.eFormat,
        (int)para->mInputFb.mRectSize.nWidth, (int)para->mInputFb.mRectSize.nHeight);
    logd("   pre_fb: addr=0x%x, 0x%x, format=%d, size=(%d, %d)",
        para->mPreFb.addr[0], para->mPreFb.addr[1], (int)para->mPreFb.eFormat,
        (int)para->mPreFb.mRectSize.nWidth, (int)para->mPreFb.mRectSize.nHeight);
    logd("output_fb: addr=0x%x, 0x%x, format=%d, size=(%d, %d)",
        para->mOutputFb.addr[0], para->mOutputFb.addr[1], (int)para->mOutputFb.eFormat,
        (int)para->mOutputFb.mRectSize.nWidth, (int)para->mOutputFb.mRectSize.nHeight);
    logd("top_field_first=%d, field=%d", para->bTopFieldFirst, para->nField);
    logd("source_regn=(%d, %d), out_regn=(%d, %d)",
        para->mSourceRegion.nWidth, para->mSourceRegion.nHeight,
        para->mOutRegion.nWidth, para->mOutRegion.nHeight);

    logd("****************************end*******************************");
    logd("**************************************************************\n\n");
    return 0;
}

static int dumpPara2(DiParaT2 *para)
{
    logd("**************************************************************");
    logd("*********************deinterlace info*************************");

    logd(" input_fb: fd=%d, addr[0~2]=0x%" PRIx64 ", 0x%" PRIx64 ", 0x%" PRIx64 ", format=%d, size=(%d, %d)",
        para->mInputFb.fd, para->mInputFb.addr[0], para->mInputFb.addr[1], para->mInputFb.addr[2],
		(int)para->mInputFb.eFormat,
		(int)para->mInputFb.mRectSize.nWidth,
		(int)para->mInputFb.mRectSize.nHeight);

    logd("   pre_fb: fd=%d, addr[0~2]=0x%" PRIx64 ", 0x%" PRIx64 ", 0x%" PRIx64 ", format=%d, size=(%d, %d)",
        para->mPreFb.fd, para->mPreFb.addr[0], para->mPreFb.addr[1], para->mPreFb.addr[2],
		(int)para->mPreFb.eFormat,
		(int)para->mPreFb.mRectSize.nWidth,
		(int)para->mPreFb.mRectSize.nHeight);

    logd("  next_fb: fd=%d, addr[0~2]=0x%" PRIx64 ", 0x%" PRIx64 ", 0x%" PRIx64 ", format=%d, size=(%d, %d)",
        para->mNextFb.fd, para->mNextFb.addr[0], para->mNextFb.addr[1], para->mNextFb.addr[2],
		(int)para->mNextFb.eFormat,
		(int)para->mNextFb.mRectSize.nWidth,
		(int)para->mNextFb.mRectSize.nHeight);

    logd("output_fb: fd=%d, addr[0~2]=0x%" PRIx64 ", 0x%" PRIx64 ", 0x%" PRIx64 ", format=%d, size=(%d, %d)",
        para->mOutputFb.fd, para->mOutputFb.addr[0], para->mOutputFb.addr[1], para->mOutputFb.addr[2],
		(int)para->mOutputFb.eFormat,
		(int)para->mOutputFb.mRectSize.nWidth,
		(int)para->mOutputFb.mRectSize.nHeight);

    logd("top_field_first=%d, field=%d", para->bTopFieldFirst, para->nField);
    logd("source_regn=(%d, %d), out_regn=(%d, %d)",
        para->mSourceRegion.nWidth, para->mSourceRegion.nHeight,
        para->mOutRegion.nWidth, para->mOutRegion.nHeight);

    logd("****************************end*******************************");
    logd("**************************************************************\n\n");
    return 0;
}

static int getIOMemType() {
    char tmp[PROP_VALUE_MAX];
    int ret = property_get("ro.kernel.iomem.type", tmp, "0");
    if (ret) {
        if (!strcmp(tmp, "0xaf10"))
            return IOMEM_IOMMU;
        else if (!strcmp(tmp, "0xfa01"))
            return IOMEM_CMA;
        else
            return IOMEM_UNKNOWN;
    }
    return IOMEM_CMA;
}

static void __DiDestroy(Deinterlace* di)
{
    struct DiContext* dc;
    dc = (struct DiContext*)di;
    if (dc->fd != -1)
    {
        close(dc->fd);
    }
    free(dc);
}

static int __DiInit(Deinterlace* di)
{
    struct DiContext* dc;
    dc = (struct DiContext*)di;

    logd("%s", __FUNCTION__);
    if (dc->fd != -1)
    {
        logw("already init...");
        return 0;
    }

    dc->fd = open("/dev/deinterlace", O_RDWR);
    if (dc->fd == -1)
    {
        loge("open hw devices failure, errno(%d)", errno);
        return -1;
    }
    dc->picCount = 0;
    dc->iomemType = getIOMemType();
    logd("iomem type=%d", dc->iomemType);
    logd("hw deinterlace init success...");
    return 0;
}

static int __DiReset(Deinterlace* di)
{
    struct DiContext* dc = (struct DiContext*)di;

    logd("%s", __FUNCTION__);
    if (dc->fd != -1)
    {
        close(dc->fd);
        dc->fd = -1;
    }
    return __DiInit(di);
}

static enum EPIXELFORMAT __DiExpectPixelFormat(Deinterlace* di)
{
    struct DiContext* dc = (struct DiContext*)di;

    return PIXEL_FORMAT_NV21;
}

static int __DiFlag(Deinterlace* di)
{
    struct DiContext* dc = (struct DiContext*)di;

    return DE_INTERLACE_HW;
}

static int __DiProcess(Deinterlace* di,
                    VideoPicture *pPrePicture,
                    VideoPicture *pCurPicture,
                    VideoPicture *pOutPicture,
                    int nField)
{
    struct DiContext* dc;
    dc = (struct DiContext*)di;

    logv("call DeinterlaceProcess");

    if(pPrePicture == NULL || pCurPicture == NULL || pOutPicture == NULL)
    {
        loge("the input param is null : %p, %p, %p", pPrePicture, pCurPicture, pOutPicture);
        return -1;
    }

    if(dc->fd == -1)
    {
        loge("not init...");
        return -1;
    }

    DiParaT        mDiParaT;
    DiRectSizeT    mSrcSize;
    DiRectSizeT    mDstSize;
    DiPixelformatE eInFormat;
    DiPixelformatE eOutFormat;

    //* compute pts again
    if (dc->picCount < 2)
    {
        int nFrameRate = 30000; //* we set the frameRate to 30
        pOutPicture->nPts = pCurPicture->nPts + nField * (1000 * 1000 * 1000 / nFrameRate) / 2;
    }
    else
    {
        pOutPicture->nPts = pCurPicture->nPts + nField * (pCurPicture->nPts - pPrePicture->nPts)/2;
    }

    logv("pCurPicture->nPts = %lld  ms, pOutPicture->nPts = %lld ms, diff = %lld ms ",
          pCurPicture->nPts/1000,
          pOutPicture->nPts/1000,
          (pOutPicture->nPts -  pCurPicture->nPts)/1000
          );

    if (pOutPicture->ePixelFormat == PIXEL_FORMAT_NV12)
    {
        eOutFormat = DI_FORMAT_NV12;
    }
    else if (pOutPicture->ePixelFormat == PIXEL_FORMAT_NV21)
    {
        eOutFormat = DI_FORMAT_NV21;
    }
    else
    {
        loge("the outputPixelFormat is not support : %d",pOutPicture->ePixelFormat);
        return -1;
    }

    const char *str_difmt = GetConfigParamterString("deinterlace_fmt", NULL);
    CDX_LOG_CHECK(str_difmt, "'deinterlace_fmt' not define, pls check your cedarx.conf");
    if (strcmp(str_difmt, "mb32") == 0)
    {
        eInFormat = DI_FORMAT_MB32_12;
    }
    else if (strcmp(str_difmt, "nv") == 0)
    {
        if (pCurPicture->ePixelFormat == PIXEL_FORMAT_NV12)
        {
            eInFormat = DI_FORMAT_NV12;
        }
        else if (pCurPicture->ePixelFormat == PIXEL_FORMAT_NV21)
        {
            eInFormat = DI_FORMAT_NV21;
        }
        else
        {
            loge("the inputPixelFormat is not support : %d",pCurPicture->ePixelFormat);
            return -1;
        }
    }
    else if (strcmp(str_difmt, "nv21") == 0)
    {
        eInFormat = DI_FORMAT_NV21;
    }
    else
    {
        eInFormat = DI_FORMAT_NV12;
    }

    mSrcSize.nWidth  = pCurPicture->nWidth;
    mSrcSize.nHeight = pCurPicture->nHeight;
    mDstSize.nWidth  = pOutPicture->nLineStride;
    mDstSize.nHeight = pOutPicture->nHeight;
    mDiParaT.mInputFb.mRectSize  = mSrcSize;
    mDiParaT.mInputFb.eFormat    = eInFormat;
    mDiParaT.mPreFb.mRectSize    = mSrcSize;
    mDiParaT.mPreFb.eFormat      = eInFormat;
    mDiParaT.mOutputFb.mRectSize = mDstSize;
    mDiParaT.mOutputFb.eFormat   = eOutFormat;
    mDiParaT.mSourceRegion       = mSrcSize;
    mDiParaT.mOutRegion          = mSrcSize;
    mDiParaT.nField              = nField;
    mDiParaT.bTopFieldFirst      = pCurPicture->bTopFieldFirst;

    //* we can use the phy address
    mDiParaT.mInputFb.addr[0]    = pCurPicture->phyYBufAddr + CONF_VE_PHY_OFFSET;
    mDiParaT.mInputFb.addr[1]    = pCurPicture->phyCBufAddr + CONF_VE_PHY_OFFSET;
    mDiParaT.mPreFb.addr[0]      = pPrePicture->phyYBufAddr + CONF_VE_PHY_OFFSET;
    mDiParaT.mPreFb.addr[1]      = pPrePicture->phyCBufAddr + CONF_VE_PHY_OFFSET;
    mDiParaT.mOutputFb.addr[0]   = pOutPicture->phyYBufAddr + CONF_VE_PHY_OFFSET;
    mDiParaT.mOutputFb.addr[1]   = pOutPicture->phyCBufAddr + CONF_VE_PHY_OFFSET;

    logv("VideoRender_CopyFrameToGPUBuffer aw_di_setpara start");

    //dumpPara(&mDiParaT);

    if (ioctl(dc->fd, DI_IOCSTART, &mDiParaT) != 0)
    {
        loge("aw_di_setpara failed!");
        return -1;
    }
    dc->picCount++;

    return 0;
}

static int __DiProcess2(Deinterlace* di,
                    VideoPicture *pPrePicture,
                    VideoPicture *pCurPicture,
                    VideoPicture *pNextPicture,
                    VideoPicture *pOutPicture,
                    int nField)
{
    struct DiContext* dc;
    dc = (struct DiContext*)di;

    logv("call DeinterlaceProcess2");

    if(pPrePicture == NULL || pCurPicture == NULL || pNextPicture == NULL || pNextPicture == NULL)
    {
        loge("the input param is null : %p, %p, %p, %p", pPrePicture, pCurPicture, pNextPicture, pOutPicture);
        return -1;
    }

    if(dc->fd == -1)
    {
        loge("not init...");
        return -1;
    }

    if (dc->iomemType == IOMEM_UNKNOWN)
    {
        loge("iomem type unknown.");
        return -1;
    }

    DiParaT2       mDiParaT2;
    DiRectSizeT    mSrcSize;
    DiRectSizeT    mVideoSize;
    DiRectSizeT    mDstSize;
    DiPixelformatE2 eInFormat;
    DiPixelformatE2 eOutFormat;

    //* compute pts again
    if (dc->picCount < 2)
    {
        int nFrameRate = 30000; //* we set the frameRate to 30
        pOutPicture->nPts = pCurPicture->nPts + nField * (1000 * 1000 * 1000 / nFrameRate) / 2;
    }
    else
    {
        pOutPicture->nPts = pCurPicture->nPts + nField * (pCurPicture->nPts - pPrePicture->nPts)/2;
    }

    logv("pCurPicture->nPts = %lld  ms, pOutPicture->nPts = %lld ms, diff = %lld ms ",
          pCurPicture->nPts/1000,
          pOutPicture->nPts/1000,
          (pOutPicture->nPts -  pCurPicture->nPts)/1000
          );

    if (pCurPicture->ePixelFormat == PIXEL_FORMAT_NV12)
    {
        eInFormat = DI2_FORMAT_NV12;
    }
    else if (pCurPicture->ePixelFormat == PIXEL_FORMAT_NV21)
    {
        eInFormat = DI2_FORMAT_NV21;
    }
    else
    {
        loge("the inputPixelFormat is not support : %d",pCurPicture->ePixelFormat);
        return -1;
    }

    if (pOutPicture->ePixelFormat == PIXEL_FORMAT_NV12)
    {
        eOutFormat = DI2_FORMAT_NV12;
    }
    else if (pOutPicture->ePixelFormat == PIXEL_FORMAT_NV21)
    {
        eOutFormat = DI2_FORMAT_NV21;
    }
    else
    {
        loge("the outputPixelFormat is not support : %d",pOutPicture->ePixelFormat);
        return -1;
    }

    mSrcSize.nWidth  = pCurPicture->nWidth;
    mSrcSize.nHeight = pCurPicture->nHeight;

    mVideoSize.nWidth = dc->pVideoSize.nWidth;
    mVideoSize.nHeight = dc->pVideoSize.nHeight;
    if (mVideoSize.nWidth == 0 || mVideoSize.nHeight == 0) {
        mVideoSize.nWidth = mSrcSize.nWidth;
        mVideoSize.nHeight = mSrcSize.nHeight;
    }

    mDstSize.nWidth  = pOutPicture->nLineStride;
    mDstSize.nHeight = pOutPicture->nHeight;

    if (dc->iomemType == IOMEM_CMA) {
        mDiParaT2.mInputFb.addr[0]    = pCurPicture->phyYBufAddr + CONF_VE_PHY_OFFSET;
        mDiParaT2.mInputFb.addr[1]    = pCurPicture->phyCBufAddr + CONF_VE_PHY_OFFSET;
    } else if (dc->iomemType == IOMEM_IOMMU) {
        mDiParaT2.mInputFb.fd = pCurPicture->nBufFd;
    }
    mDiParaT2.mInputFb.mRectSize.nWidth   = mSrcSize.nWidth ;
    mDiParaT2.mInputFb.mRectSize.nHeight  = mSrcSize.nHeight;
    mDiParaT2.mInputFb.eFormat    = eInFormat;

    if (dc->iomemType == IOMEM_CMA) {
        mDiParaT2.mPreFb.addr[0]      = pPrePicture->phyYBufAddr + CONF_VE_PHY_OFFSET;
        mDiParaT2.mPreFb.addr[1]      = pPrePicture->phyCBufAddr + CONF_VE_PHY_OFFSET;
    } else if (dc->iomemType == IOMEM_IOMMU) {
        mDiParaT2.mPreFb.fd = pPrePicture->nBufFd;
    }
    mDiParaT2.mPreFb.mRectSize.nWidth   = mSrcSize.nWidth ;
    mDiParaT2.mPreFb.mRectSize.nHeight  = mSrcSize.nHeight;
    mDiParaT2.mPreFb.eFormat      = eInFormat;

    if (dc->iomemType == IOMEM_CMA) {
        mDiParaT2.mNextFb.addr[0]     = pNextPicture->phyYBufAddr + CONF_VE_PHY_OFFSET;
        mDiParaT2.mNextFb.addr[1]     = pNextPicture->phyCBufAddr + CONF_VE_PHY_OFFSET;
    } else if (dc->iomemType == IOMEM_IOMMU) {
        mDiParaT2.mNextFb.fd = pNextPicture->nBufFd;
    }
    mDiParaT2.mNextFb.mRectSize.nWidth   = mSrcSize.nWidth ;
    mDiParaT2.mNextFb.mRectSize.nHeight  = mSrcSize.nHeight;
    mDiParaT2.mNextFb.eFormat     = eInFormat;

    mDiParaT2.mSourceRegion.nWidth   = mVideoSize.nWidth;
    mDiParaT2.mSourceRegion.nHeight  = mVideoSize.nHeight;
    mDiParaT2.mOutRegion.nWidth      = mVideoSize.nWidth;
    mDiParaT2.mOutRegion.nHeight     = mVideoSize.nHeight;

    if (dc->iomemType == IOMEM_CMA) {
        mDiParaT2.mOutputFb.addr[0]   = pOutPicture->phyYBufAddr + CONF_VE_PHY_OFFSET;
        mDiParaT2.mOutputFb.addr[1]   = pOutPicture->phyCBufAddr + CONF_VE_PHY_OFFSET;
    } else if (dc->iomemType == IOMEM_IOMMU) {
        mDiParaT2.mOutputFb.fd = pOutPicture->nBufFd;
    }
    mDiParaT2.mOutputFb.mRectSize.nWidth   = mDstSize.nWidth ;
    mDiParaT2.mOutputFb.mRectSize.nHeight  = mDstSize.nHeight;
    mDiParaT2.mOutputFb.eFormat   = eOutFormat;

    mDiParaT2.nField              = nField;
    mDiParaT2.bTopFieldFirst      = pCurPicture->bTopFieldFirst;

    //dumpPara2(&mDiParaT2);

    if (ioctl(dc->fd, DI_IOCSTART2, &mDiParaT2) != 0)
    {
        loge("aw_di_setpara2 failed!");
        return -1;
    }

    dc->picCount++;

    return 0;
}

static int __DiSetVideoSize(Deinterlace* di, int w, int h)
{
    struct DiContext* dc = (struct DiContext*)di;

	logd("%s [%d x %d]", __FUNCTION__ , w, h);

	dc->pVideoSize.nWidth= w;
	dc->pVideoSize.nHeight = h;

    return 0;

}

static struct DeinterlaceOps mDi =
{
    .destroy           = __DiDestroy,
    .init              = __DiInit,
    .reset             = __DiReset,
    .expectPixelFormat = __DiExpectPixelFormat,
    .flag              = __DiFlag,
    .process           = __DiProcess,
    .process2          = __DiProcess2,
    .setVideoSize      = __DiSetVideoSize,
};

Deinterlace* DeinterlaceCreate()
{
    struct DiContext* dc = (struct DiContext*)malloc(sizeof(struct DiContext));
    if(dc == NULL)
    {
        logw("deinterlace create failed");
        return NULL;
    }
    memset(dc, 0, sizeof(struct DiContext));

    // we must set the fd to -1; or it will close the file which fd is 0
    // when destroy
    dc->fd = -1;

    dc->base.ops = &mDi;

    return &dc->base;
}
