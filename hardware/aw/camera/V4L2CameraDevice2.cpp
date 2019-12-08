/*
 * Copyright (c) 2008-2015 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "CameraDebug.h"
#if DBG_V4L2_CAMERA
#define LOG_NDEBUG 0
#endif
#define LOG_TAG "V4L2CameraDevice"
#include <cutils/log.h>

#include <sys/mman.h>
#include <camerahal/linux/videodev2_new.h>
//#include <linux/videodev.h>
#include <sys/time.h>

//TOOLS FOR CAPTURE BITSTREAM
//add for debug bitstream by zhengjiangwei
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory/sc_interface.h"

#include "V4L2CameraDevice2.h"
#include "CallbackNotifier.h"
#include "PreviewWindow.h"
#include "CameraHardware2.h"
#include "HALCameraFactory.h"
//#include "memory/ionMemory/ionAlloc.h"
#include "memory/memoryAdapter.h"


#define CHECK_NO_ERROR(a)                       \
    if (a != NO_ERROR) {                        \
        if (mCameraFd != (int)NULL) {           \
            close(mCameraFd);                   \
            mCameraFd = (int)NULL;              \
        }                                       \
        return EINVAL;                          \
    }

extern void PreviewCnr(unsigned int snr_level,
                       unsigned int gain,
                       int width,
                       int height,
                       char *src,
                       char *dst);

namespace android {

// defined in HALCameraFactory.cpp
extern void getCallingProcessName(char *name);


static void calculateCrop(Rect * rect, int new_zoom, int max_zoom, int width, int height)
{
    if (max_zoom == 0)
    {
        rect->left        = 0;
        rect->top        = 0;
        rect->right     = width -1;
        rect->bottom    = height -1;
    }
    else
    {
        int new_ratio = (new_zoom * 2 * 100 / max_zoom + 100);
        rect->left        = (width - (width * 100) / new_ratio)/2;
        rect->top        = (height - (height * 100) / new_ratio)/2;
        rect->right     = rect->left + (width * 100) / new_ratio -1;
        rect->bottom    = rect->top  + (height * 100) / new_ratio - 1;
    }

    // LOGD("crop: [%d, %d, %d, %d]", rect->left, rect->top, rect->right, rect->bottom);
}


static void YUYVToNV12(const void* yuyv, void *nv12, int width, int height)
{
    uint8_t* Y    = (uint8_t*)nv12;
    uint8_t* UV = (uint8_t*)Y + width * height;

    for(int i = 0; i < height; i += 2)
    {
        for (int j = 0; j < width; j++)
        {
            *(uint8_t*)((uint8_t*)Y + i * width + j) =
                *(uint8_t*)((uint8_t*)yuyv + i * width * 2 + j * 2);
            *(uint8_t*)((uint8_t*)Y + (i + 1) * width + j) =
                *(uint8_t*)((uint8_t*)yuyv + (i + 1) * width * 2 + j * 2);
            *(uint8_t*)((uint8_t*)UV + ((i * width) >> 1) + j) =
                *(uint8_t*)((uint8_t*)yuyv + i * width * 2 + j * 2 + 1);
        }
    }
}

static void YUYVToNV21(const void* yuyv, void *nv21, int width, int height)
{
    uint8_t* Y    = (uint8_t*)nv21;
    uint8_t* VU = (uint8_t*)Y + ALIGN_16B(width) * ALIGN_16B(height);

    for(int i = 0; i < height; i += 2)
    {
        for (int j = 0; j < width; j++)
        {
            *(uint8_t*)((uint8_t*)Y + i * width + j) =
                *(uint8_t*)((uint8_t*)yuyv + i * width * 2 + j * 2);
            *(uint8_t*)((uint8_t*)Y + (i + 1) * width + j) =
                *(uint8_t*)((uint8_t*)yuyv + (i + 1) * width * 2 + j * 2);

            if (j % 2)
            {
                if (j == (width -1))
                {
                    *(uint8_t*)((uint8_t*)VU + ((i * width) >> 1) + j) =
                        *(uint8_t*)((uint8_t*)yuyv + i * width * 2 + (j + 1) * 2 + 1);
                }
                else if (j < width - 1)
                {
                    *(uint8_t*)((uint8_t*)VU + ((i * width) >> 1) + j) =
                        *(uint8_t*)((uint8_t*)yuyv + i * width * 2 + (j + 1) * 2 + 1);
                }
            }
            else
            {
                if(j == 0)
                {
                    *(uint8_t*)((uint8_t*)VU + ((i * width) >> 1) + j) =
                        *(uint8_t*)((uint8_t*)yuyv + i * width * 2  + 3);
                }else if (j > 1)
                {
                    *(uint8_t*)((uint8_t*)VU + ((i * width) >> 1) + j) =
                        *(uint8_t*)((uint8_t*)yuyv + i * width * 2 + (j - 1) * 2 + 1);
                }
            }
        }
    }
    LOGV("zjw,the left v =VU = %d,the left u =VU+1 = %d,the right v =VU+width>>1-1 = %d,the right u =VU+width>>1= %d,"
		,*(VU),*(VU+1),*(VU+(width>>1)-1),*(VU+(width>>1)));
}


DBG_TIME_AVG_BEGIN(TAG_CONTINUOUS_PICTURE);
static int saveframe(char *str,void *p,int length,int is_oneframe)
{
    FILE *fd;

    if(is_oneframe){
        fd = fopen(str,"wb");
    }
    else{
        fd = fopen(str,"a");
    }

    if(!fd){
        LOGE("Open file error");
        return -1;
    }
    if(fwrite(p,1,length,fd)){
       LOGD("Write file successfully");
       fclose(fd);
       return 0;
    }
    else {
       LOGE("Write file fail");
       fclose(fd);
       return -1;
    }
}

void  V4L2CameraDevice::showformat(int format, const char *str)
{
    switch(format){
    case V4L2_PIX_FMT_YUYV:
        LOGD("The %s foramt is V4L2_PIX_FMT_YUYV",str);
        break;
    case V4L2_PIX_FMT_MJPEG:
        LOGD("The %s foramt is V4L2_PIX_FMT_MJPEG",str);
        break;
    case V4L2_PIX_FMT_YVU420:
        LOGD("The %s foramt is V4L2_PIX_FMT_YVU420",str);
        break;
    case V4L2_PIX_FMT_NV12:
        LOGD("The %s foramt is V4L2_PIX_FMT_NV12",str);
        break;
    case V4L2_PIX_FMT_NV21:
        LOGD("The %s foramt is V4L2_PIX_FMT_NV21",str);
        break;
    case V4L2_PIX_FMT_H264:
        LOGD("The %s foramt is V4L2_PIX_FMT_H264",str);
        break;
    default:
        LOGD("The %s format can't be showed",str);
    }
}

V4L2CameraDevice::V4L2CameraDevice(CameraHardware* camera_hal,
                                   PreviewWindow * preview_window,
                                   CallbackNotifier * cb)
    : mCameraHardware(camera_hal),
      mPreviewWindow(preview_window),
      mCallbackNotifier(cb),
      mCameraDeviceState(STATE_CONSTRUCTED),
      mCaptureThreadState(CAPTURE_STATE_NULL),
      mCameraFd(0),
      mIsUsbCamera(false),
      mFrameRate(30),
      mTakePictureState(TAKE_PICTURE_NULL),
      mIsPicCopy(false),
      mFrameWidth(0),
      mFrameHeight(0),
      mThumbWidth(0),
      mThumbHeight(0),
      mCurFrameTimestamp(0),
      mBufferCnt(NB_BUFFER),
      mUseHwEncoder(false),
      mNewZoom(0),
      mLastZoom(-1),
      mMaxZoom(0xffffffff),
      mCaptureFormat(V4L2_PIX_FMT_NV21),
      mVideoFormat(V4L2_PIX_FMT_NV21)
      ,mCurrentV4l2buf(NULL)
      ,mCanBeDisconnected(false)
      ,mContinuousPictureStarted(false)
      ,mContinuousPictureCnt(0)
      ,mContinuousPictureMax(0)
      ,mContinuousPictureStartTime(0)
      ,mSmartPictureDone(true)
      ,mContinuousPictureLast(0)
      ,mContinuousPictureAfter(0)
      ,mFaceDectectLast(0)
      ,mFaceDectectAfter(0)
      ,mPreviewLast(0)
      ,mPreviewAfter(0)
      ,mVideoHint(false)
      ,mDecoder(NULL)
      ,mCurAvailBufferCnt(0)
      ,mStatisicsIndex(0)
      ,mNeedHalfFrameRate(false)
      ,mShouldPreview(true)
      ,mIsThumbUsedForVideo(false)
      ,mVideoWidth(640)
      ,mVideoHeight(480)
      ,mMemOpsS(NULL)
      ,mDiscardFrameNum(0)
{
    LOGV("V4L2CameraDevice construct");

    memset(&mHalCameraInfo, 0, sizeof(mHalCameraInfo));
    memset(&mRectCrop, 0, sizeof(Rect));
    memset(&mMapMem, 0, sizeof(v4l2_mem_from_device_t));
    memset(&mVideoConf, 0, sizeof(VConfig));
    memset(&mVideoInfo, 0, sizeof(VideoStreamInfo));
    memset(&mDataInfo, 0, sizeof(VideoStreamDataInfo));

    for(int i = 0; i < NB_BUFFER;i++)
    {
        mbufferUnit[i].buffer_state = false;
        mbufferUnit[i].delete_handle_state = false;
        mbufferUnit[i].mbufferUnitMetadata = NULL;
    }

#if 1 //jiangwei
    mMemOpsS = MemCamAdapterGetOpsS();
    mMemOpsS->open_cam();
#endif
   // ion_alloc_open();

    // init preview buffer queue
    OSAL_QueueCreate(&mQueueBufferPreview, NB_BUFFER);
    OSAL_QueueCreate(&mQueueBufferPicture, 4);

    // init capture thread
    mCaptureThread = new DoCaptureThread(this);
    pthread_mutex_init(&mCaptureMutex, NULL);
    pthread_cond_init(&mCaptureCond, NULL);
    mCaptureThreadState = CAPTURE_STATE_PAUSED;
    mCaptureThread->startThread();

    // init preview thread
    mPreviewThread = new DoPreviewThread(this);
    pthread_mutex_init(&mPreviewMutex, NULL);
    pthread_cond_init(&mPreviewCond, NULL);
    mPreviewThread->startThread();

    // init picture thread
    mPictureThread = new DoPictureThread(this);
    pthread_mutex_init(&mPictureMutex, NULL);
    pthread_cond_init(&mPictureCond, NULL);
    mPictureThread->startThread();

    pthread_mutex_init(&mConnectMutex, NULL);
    pthread_cond_init(&mConnectCond, NULL);

    // init continuous picture thread
    mContinuousPictureThread = new DoContinuousPictureThread(this);
    pthread_mutex_init(&mContinuousPictureMutex, NULL);
    pthread_cond_init(&mContinuousPictureCond, NULL);
    mContinuousPictureThread->startThread();

    // init smart picture thread
    mSmartPictureThread = new DoSmartPictureThread(this);
    pthread_mutex_init(&mSmartPictureMutex, NULL);
    pthread_cond_init(&mSmartPictureCond, NULL);
    mSmartPictureThread->startThread();
}

V4L2CameraDevice::~V4L2CameraDevice()
{
    LOGV("V4L2CameraDevice disconstruct");

    if (mCaptureThread != NULL)
    {
        mCaptureThread->stopThread();
        pthread_cond_signal(&mCaptureCond);
        mCaptureThread.clear();
        mCaptureThread = 0;
    }

    if (mPreviewThread != NULL)
    {
        mPreviewThread->stopThread();
        pthread_cond_signal(&mPreviewCond);
        mPreviewThread.clear();
        mPreviewThread = 0;
    }

    if (mPictureThread != NULL)
    {
        mPictureThread->stopThread();
        pthread_cond_signal(&mPictureCond);
        mPictureThread.clear();
        mPictureThread = 0;
    }

    if (mContinuousPictureThread != NULL)
    {
        mContinuousPictureThread->stopThread();
        pthread_cond_signal(&mContinuousPictureCond);
        mContinuousPictureThread.clear();
        mContinuousPictureThread = 0;
    }

    if (mSmartPictureThread != NULL)
    {
        mSmartPictureThread->stopThread();
        pthread_cond_signal(&mSmartPictureCond);
        mSmartPictureThread.clear();
        mSmartPictureThread = 0;
    }
    if (mMemOpsS != NULL)
    {
        mMemOpsS->close_cam();
    }

    pthread_mutex_destroy(&mCaptureMutex);
    pthread_cond_destroy(&mCaptureCond);

    pthread_mutex_destroy(&mPreviewMutex);
    pthread_cond_destroy(&mPreviewCond);

    pthread_mutex_destroy(&mPictureMutex);
    pthread_cond_destroy(&mPictureCond);

    pthread_mutex_destroy(&mConnectMutex);
    pthread_cond_destroy(&mConnectCond);

    pthread_mutex_destroy(&mContinuousPictureMutex);
    pthread_cond_destroy(&mContinuousPictureCond);

    pthread_mutex_destroy(&mSmartPictureMutex);
    pthread_cond_destroy(&mSmartPictureCond);

    OSAL_QueueTerminate(&mQueueBufferPreview);
    OSAL_QueueTerminate(&mQueueBufferPicture);
}

/****************************************************************************
 * V4L2CameraDevice interface implementation.
 ***************************************************************************/

status_t V4L2CameraDevice::connectDevice(HALCameraInfo * halInfo)
{
    F_LOG;

    Mutex::Autolock locker(&mObjectLock);

    if (isConnected())
    {
        LOGW("%s: camera device is already connected.", __FUNCTION__);
        return NO_ERROR;
    }

    // open v4l2 camera device
    int ret = openCameraDev(halInfo);
    if (ret != OK)
    {
        return ret;
    }

    memcpy((void*)&mHalCameraInfo, (void*)halInfo, sizeof(HALCameraInfo));
/*
    ret = ion_alloc_open();
    if (ret < 0)
    {
        LOGE("ion_alloc_open failed");
        goto END_ERROR;
    }
    LOGV("ion_alloc_open ok");
*/
    // used for normal picture mode
    mPicBuffer.addrVirY = (unsigned long)mMemOpsS->palloc_cam(MAX_PICTURE_SIZE, SHARE_BUFFER_FD,&mPicBuffer.nShareBufFd);
    LOGD("zjw,connectDevice mPicBuffer.nShareBufFd = %d \n",mPicBuffer.nShareBufFd);

    /* There is a device to connect to. */
    mCameraDeviceState = STATE_CONNECTED;

    return NO_ERROR;

END_ERROR:

    if (mCameraFd != (int)NULL)
    {
        close(mCameraFd);
        mCameraFd = (int)NULL;
    }


    return UNKNOWN_ERROR;
}

status_t V4L2CameraDevice::disconnectDevice()
{
    F_LOG;

    Mutex::Autolock locker(&mObjectLock);

    if (!isConnected())
    {
        LOGW("%s: camera device is already disconnected.", __FUNCTION__);
        return NO_ERROR;
    }

    if (isStarted())
    {
        LOGE("%s: Cannot disconnect from the started device.", __FUNCTION__);
        return -EINVAL;
    }

    // close v4l2 camera device
    closeCameraDev();

    if (mPicBuffer.addrVirY != NULL)
    {
        mMemOpsS->pfree_cam((void*)mPicBuffer.addrVirY);
        mPicBuffer.addrPhyY = 0;
    }
/*
    int ret = ion_alloc_close();
    if (ret < 0)
    {
        LOGE("sunxi_alloc_close failed\n");
    }
    else
    {
        LOGV("sunxi_alloc_close ok");
    }
*/
    /* There is no device to disconnect from. */
    mCameraDeviceState = STATE_CONSTRUCTED;

    return NO_ERROR;
}

status_t V4L2CameraDevice::startDevice(int width,
                                       int height,
                                       uint32_t pix_fmt,
                                       bool video_hint)
{
    LOGD("%s, wxh: %dx%d, fmt: %d", __FUNCTION__, width, height, pix_fmt);

    Mutex::Autolock locker(&mObjectLock);

    if (!isConnected())
    {
        LOGE("%s: camera device is not connected.", __FUNCTION__);
        return EINVAL;
    }

    if (isStarted())
    {
        LOGE("%s: camera device is already started.", __FUNCTION__);
        return EINVAL;
    }

    // VE encoder need this format
    mVideoFormat = pix_fmt;
    mCurrentV4l2buf = NULL;

    mVideoHint = video_hint;
    mCanBeDisconnected = false;

//TOOLS FOR CAPTURE BITSTREAM
//We open the file in there ,if we want to capture the picture(one frame),
//we must change the resolution in APP UI,because
//the V4L2CameraDevice::startDevice only work in  mCaptureWidth != frame_width&& mCaptureHeight != frame_height..
//See status_t CameraHardware::doTakePicture() you can get more details..
#if DBG_CAPTURE_STREAM_FROM_V4L2DEVICE_TAKEPICTURE||DBG_CAPTURE_STREAM_FROM_V4L2DEVICE_FORNV21||DBG_CAPTURE_STREAM_FROM_V4L2DEVICE_FORYUYV||DBG_CAPTURE_STREAM_FROM_V4L2DEVICE_FORMJPEG||DBG_CAPTURE_STREAM_FROM_V4L2DEVICE_FORH264
    char fname0[128] ;
    char fname0_tail[128];
    frame_num0 = 0;
    v4l2device_tail_num++;

    sprintf(fname0,DBG_CAPTURE_V4L2DEVICE_PATH);
    sprintf(fname0_tail,"%d",v4l2device_tail_num);

    strcat(fname0,fname0_tail);

    fp_stream_from_v4l2device = fopen(fname0,"w");
    if(fp_stream_from_v4l2device == NULL)
    {
        LOGE("%s: zheng jiangwei,Open DBG_CAPTURE_V4L2DEVICE_PATH fail!", __FUNCTION__);
    }
#endif

#if DBG_CAPTURE_STREAM_AFTER_TRANSFORMATION
    char fname1[128] ;
    char fname1_tail[128];
    frame_num1 = 0;
    afterTransformation_tail_num++;

    sprintf(fname1,DBG_CAPTURE_AFTER_TRANSFORMATION_PATH);
    sprintf(fname1_tail,"%d",afterTransformation_tail_num);

    strcat(fname1,fname1_tail);

    fp_stream_after_transformation = fopen(fname1,"w");
    if(fp_stream_after_transformation == NULL)
    {
        LOGE("%s: zheng jiangwei,Open DBG_CAPTURE_AFTER_TRANSFORMATION_PATH fail!", __FUNCTION__);
    }
#endif

    // set capture mode and fps
    // CHECK_NO_ERROR(v4l2setCaptureParams());    // do not check this error
    v4l2setCaptureParams();

    // set v4l2 device parameters, it maybe change the value of mFrameWidth and mFrameHeight.
    CHECK_NO_ERROR(v4l2SetVideoParams(width, height, pix_fmt));

    // v4l2 request buffers
    int buf_cnt = (mTakePictureState == TAKE_PICTURE_NORMAL) ? 1 : NB_BUFFER;
    CHECK_NO_ERROR(v4l2ReqBufs(&buf_cnt));
    mBufferCnt = buf_cnt;
    mCurAvailBufferCnt = mBufferCnt;

    LOGD("window allocate");
    mPreviewWindow->allocate(mBufferCnt,
		width,
		height,
		pix_fmt);
    mPreviewWindow->enqueue_buffer_num = 0;


    // v4l2 query buffers
    CHECK_NO_ERROR(v4l2QueryBuf());

    // stream on the v4l2 device
    CHECK_NO_ERROR(v4l2StartStreaming());

    mCameraDeviceState = STATE_STARTED;

    mContinuousPictureAfter = 1000000 / 10;
    mFaceDectectAfter = 1000000 / 15;
    mPreviewAfter = 1000000 / 24;


    if((mCaptureFormat == V4L2_PIX_FMT_MJPEG)||(mCaptureFormat == V4L2_PIX_FMT_H264))
    {
       // LOGD("FUNC:%s, Line:%d init Dec!,mMemOpsS :%x ",__FUNCTION__,__LINE__,mMemOpsS);
        mVideoConf.memops = NULL;
        //* all decoder support YV12 format. (PIXEL_FORMAT_YV12;PIXEL_FORMAT_YUV_MB32_420)
        mVideoConf.eOutputPixelFormat  = PIXEL_FORMAT_NV21;
        //* never decode two picture when decoding a thumbnail picture.
        mVideoConf.bDisable3D          = 1;

        //added by zhengjiangwei ,fix decoder H264 bitstream that is wrong,and to display normally.
        mVideoConf.bDispErrorFrame = 1;

        mVideoConf.nVbvBufferSize      = 0;

        mVideoInfo.eCodecFormat = (mCaptureFormat == V4L2_PIX_FMT_MJPEG) ?
                                  VIDEO_CODEC_FORMAT_MJPEG :
                                  VIDEO_CODEC_FORMAT_H264;

        mVideoInfo.nWidth = mFrameWidth;;
        mVideoInfo.nHeight = mFrameHeight;
        mVideoInfo.nFrameRate = mFrameRate;
        mVideoInfo.nFrameDuration = 1000*1000/mFrameRate;
        mVideoInfo.nAspectRatio = 1000;
        mVideoInfo.bIs3DStream = 0;
        mVideoInfo.nCodecSpecificDataLen = 0;
        mVideoInfo.pCodecSpecificData = NULL;

        Libve_init2(&mDecoder,&mVideoInfo,&mVideoConf);
        if(mDecoder == NULL){
            LOGE("FUNC:%s, Line:%d ",__FUNCTION__,__LINE__);
        }
    }

    mDiscardFrameNum = 0;
    return NO_ERROR;
}

status_t V4L2CameraDevice::stopDevice()
{
    LOGD("V4L2CameraDevice::stopDevice");

    pthread_mutex_lock(&mConnectMutex);
    if (!mCanBeDisconnected)
    {
        LOGW("wait until capture thread pause or exit");
        pthread_cond_wait(&mConnectCond, &mConnectMutex);
    }
    pthread_mutex_unlock(&mConnectMutex);

    Mutex::Autolock locker(&mObjectLock);

    if (!isStarted())
    {
        LOGW("%s: camera device is not started.", __FUNCTION__);
        return NO_ERROR;
    }
//TOOLS FOR CAPTURE BITSTREAM
//When the device close ,we also close the file...
#if DBG_CAPTURE_STREAM_FROM_V4L2DEVICE_TAKEPICTURE||DBG_CAPTURE_STREAM_FROM_V4L2DEVICE_FORNV21||DBG_CAPTURE_STREAM_FROM_V4L2DEVICE_FORYUYV||DBG_CAPTURE_STREAM_FROM_V4L2DEVICE_FORMJPEG||DBG_CAPTURE_STREAM_FROM_V4L2DEVICE_FORH264
    fclose(fp_stream_from_v4l2device);
#endif
#if DBG_CAPTURE_STREAM_AFTER_TRANSFORMATION
    fclose(fp_stream_after_transformation);
#endif

    // v4l2 device stop stream
    v4l2StopStreaming();

#if USE_CSI_VIN_DRIVER
    for(int i = 0; i < mBufferCnt; i++)
    {
        mMemOpsS->pfree_cam(mMapMem.mem[i]);
        mMapMem.mem[i] = NULL;

    }
#else
     // v4l2 device unmap buffers
     v4l2UnmapBuf();
     mPreviewWindow->freeBufferHandle(mBufferCnt,mPreviewWindow->mBufferHandle);
     LOGD("%s: mPreviewWindow->freeBufferHandle finish.", __FUNCTION__);
#endif

    for(int i = 0; i < NB_BUFFER; i++)
    {
        memset(&mV4l2buf[i], 0, sizeof(V4L2BUF_t));
    }

    mCameraDeviceState = STATE_CONNECTED;

    mLastZoom = -1;

    mCurrentV4l2buf = NULL;

    if(mCaptureFormat == V4L2_PIX_FMT_MJPEG || mCaptureFormat == V4L2_PIX_FMT_H264)
    {
        Libve_exit2(&mDecoder);
    }

#if 0
    for(int i = 0;i<NB_BUFFER;i++)
    {
       int ret = -1;

	if(mbufferUnit[i].delete_handle_state)
	{

		mbufferUnit[i].delete_handle_state = false;

		LOGD("zjw,in previewThread check mbufferUnit[%d].delete_handle_state\n",i);


	   ret = native_handle_close(mbufferUnit[i].mbufferUnitMetadata);
	   if(ret != 0)
	   {
		LOGD("zjw,in previewThread	native_handle_close error  = %d\n",ret);
	   }
	   ret = native_handle_delete(mbufferUnit[i].mbufferUnitMetadata);
	   if(ret != 0)
	   {
		LOGD("zjw,in previewThread	native_handle_delete error	= %d\n",ret);
	   }
	}
    }
#endif
    return NO_ERROR;
}

status_t V4L2CameraDevice::startDeliveringFrames()
{
    F_LOG;

    pthread_mutex_lock(&mCaptureMutex);

    if (mCaptureThreadState == CAPTURE_STATE_NULL)
    {
        LOGE("error state of capture thread, %s", __FUNCTION__);
        pthread_mutex_unlock(&mCaptureMutex);
        return EINVAL;
    }

    if (mCaptureThreadState == CAPTURE_STATE_STARTED)
    {
        LOGW("capture thread has already started");
        pthread_mutex_unlock(&mCaptureMutex);
        return NO_ERROR;
    }

    // singal to start capture thread
    mCaptureThreadState = CAPTURE_STATE_STARTED;
    pthread_cond_signal(&mCaptureCond);
    pthread_mutex_unlock(&mCaptureMutex);

    return NO_ERROR;
}

status_t V4L2CameraDevice::stopDeliveringFrames()
{
    F_LOG;

    pthread_mutex_lock(&mCaptureMutex);
    if (mCaptureThreadState == CAPTURE_STATE_NULL)
    {
        LOGE("error state of capture thread, %s", __FUNCTION__);
        pthread_mutex_unlock(&mCaptureMutex);
        return EINVAL;
    }

    if (mCaptureThreadState == CAPTURE_STATE_PAUSED)
    {
        LOGW("capture thread has already paused");
        pthread_mutex_unlock(&mCaptureMutex);
        return NO_ERROR;
    }

    mCaptureThreadState = CAPTURE_STATE_PAUSED;
    pthread_mutex_unlock(&mCaptureMutex);

    return NO_ERROR;
}


/****************************************************************************
 * Worker thread management.
 ***************************************************************************/

int V4L2CameraDevice::v4l2WaitCameraReady()
{
    fd_set fds;
    struct timeval tv;
    int r;

    FD_ZERO(&fds);
    FD_SET(mCameraFd, &fds);

    /* Timeout */
    tv.tv_sec  = 2;
    tv.tv_usec = 0;

    r = select(mCameraFd + 1, &fds, NULL, NULL, &tv);
    if (r == -1)
    {
        LOGE("select err, %s", strerror(errno));
        return -1;
    }
    else if (r == 0)
    {
        LOGE("select timeout");
        return -1;
    }

    return 0;
}

void V4L2CameraDevice::singalDisconnect()
{
    pthread_mutex_lock(&mConnectMutex);
    mCanBeDisconnected = true;
    pthread_cond_signal(&mConnectCond);
    pthread_mutex_unlock(&mConnectMutex);
}

bool V4L2CameraDevice::captureThread()
{
    pthread_mutex_lock(&mCaptureMutex);
    // stop capture
    if (mCaptureThreadState == CAPTURE_STATE_PAUSED)
    {
        singalDisconnect();
        // wait for signal of starting to capture a frame
        LOGV("capture thread paused");
        pthread_cond_wait(&mCaptureCond, &mCaptureMutex);
    }

    // thread exit
    if (mCaptureThreadState == CAPTURE_STATE_EXIT)
    {
        singalDisconnect();
        LOGV("capture thread exit");
        pthread_mutex_unlock(&mCaptureMutex);
        return false;
    }
    pthread_mutex_unlock(&mCaptureMutex);

    int ret = v4l2WaitCameraReady();

    pthread_mutex_lock(&mCaptureMutex);
    // stop capture or thread exit
    if (mCaptureThreadState == CAPTURE_STATE_PAUSED
        || mCaptureThreadState == CAPTURE_STATE_EXIT)
    {
        singalDisconnect();
        LOGW("should stop capture now");
        pthread_mutex_unlock(&mCaptureMutex);
        return __LINE__;
    }

    if (ret != 0)
    {
        LOGW("wait v4l2 buffer time out");
        pthread_mutex_unlock(&mCaptureMutex);

        LOGW("preview queue has %d items.", OSAL_GetElemNum(&mQueueBufferPreview));
        return __LINE__;
    }

    // get one video frame
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(v4l2_buffer));
    ret = getPreviewFrame(&buf);

    LOGV("zjw,time after getPreviewFrame = %lld,",systemTime() / 1000000);
    LOGV("zjw,getPreviewFrame buffer index = %d,",buf.index);

    if (ret != OK)
    {
        pthread_mutex_unlock(&mCaptureMutex);

        usleep(10000);
        return ret;
    }

    mCurAvailBufferCnt--;
    LOGV("zjw,after getPreviewFrame buffer and the camera kernal has the number of buffer = %d,",mCurAvailBufferCnt);

    if (mCurAvailBufferCnt <= 4)
    {
        mNeedHalfFrameRate = true;
        mStatisicsIndex = 0;
    }
    else if (mNeedHalfFrameRate)
    {
        mStatisicsIndex++;
        if (mStatisicsIndex >= STATISICS_CNT)
        {
            mNeedHalfFrameRate = false;
        }
    }

    // deal with this frame
    //mCurFrameTimestamp = (int64_t)((int64_t)buf.timestamp.tv_usec +
    //                     (((int64_t)buf.timestamp.tv_sec) * 1000000));

    mCurFrameTimestamp = (int64_t)systemTime();

    if (mLastZoom != mNewZoom)
    {
        float widthRate = (float)mFrameWidth / (float)mVideoWidth;
        float heightRate = (float)mFrameHeight / (float)mVideoHeight;
        if (mIsThumbUsedForVideo && (widthRate > 4.0) && (heightRate > 4.0))
        {
            // for cts, to do
            calculateCrop(&mRectCrop, mNewZoom, mMaxZoom, mFrameWidth/2, mFrameHeight/2);
        }
        else
        {
            // the main frame crop
            calculateCrop(&mRectCrop, mNewZoom, mMaxZoom, mFrameWidth, mFrameHeight);
        }
        mCameraHardware->setNewCrop(&mRectCrop);

        // the sub-frame crop
        if (mHalCameraInfo.fast_picture_mode)
        {
            calculateCrop(&mThumbRectCrop, mNewZoom, mMaxZoom, mThumbWidth, mThumbHeight);
        }

        mLastZoom = mNewZoom;

        LOGV("CROP: [%d, %d, %d, %d]", mRectCrop.left,
                                       mRectCrop.top,
                                       mRectCrop.right,
                                       mRectCrop.bottom);
        LOGV("thumb CROP: [%d, %d, %d, %d]", mThumbRectCrop.left,
                                             mThumbRectCrop.top,
                                             mThumbRectCrop.right,
                                             mThumbRectCrop.bottom);
    }

    if((mCaptureFormat == V4L2_PIX_FMT_MJPEG) || (mCaptureFormat == V4L2_PIX_FMT_H264))
    {
        mDataInfo.nLength        =buf.bytesused;
        mDataInfo.nPts            =(int64_t)mCurFrameTimestamp/1000;
        mMemOpsS->flush_cache_cam((void*)mPreviewWindow->ddr_vir[buf.index],
                              mFrameWidth*mFrameHeight*3/2);

        Libve_dec2(&mDecoder,
            mMapMem.mem[buf.index],
            (void*)mPreviewWindow->ddr_vir[buf.index],
            &mVideoInfo,
            &mDataInfo,
            &mVideoConf);

        mMemOpsS->flush_cache_cam((void*)mPreviewWindow->ddr_vir[buf.index],
                              mFrameWidth*mFrameHeight*3/2);
    }

    if (mVideoFormat != V4L2_PIX_FMT_YUYV
        && mCaptureFormat == V4L2_PIX_FMT_YUYV)
    {
        mMemOpsS->flush_cache_cam((void*)mPreviewWindow->ddr_vir[buf.index],
                              ALIGN_16B(mFrameWidth) * ALIGN_16B(mFrameHeight) * 3/2);

        YUYVToNV21(mMapMem.mem[buf.index],
                       (void*)mPreviewWindow->ddr_vir[buf.index],
                       mFrameWidth,
                       mFrameHeight);
        mMemOpsS->flush_cache_cam((void*)mPreviewWindow->ddr_vir[buf.index],
                              ALIGN_16B(mFrameWidth) * ALIGN_16B(mFrameHeight) * 3/2);
    }

    // V4L2BUF_t for preview and HW encoder
    V4L2BUF_t v4l2_buf;
    if (mVideoFormat != V4L2_PIX_FMT_YUYV
        && mCaptureFormat == V4L2_PIX_FMT_YUYV)
    {
        // may be (addr - 0x20000000) or (addr & 0x0fffffff)
        v4l2_buf.addrVirY       = (unsigned long)mPreviewWindow->ddr_vir[buf.index];
        v4l2_buf.width          = mFrameWidth;
        v4l2_buf.height         = mFrameHeight;
    }
    else if(mVideoFormat != V4L2_PIX_FMT_YUYV
        &&( (mCaptureFormat == V4L2_PIX_FMT_MJPEG) || (mCaptureFormat == V4L2_PIX_FMT_H264) ))
    {
        // may be (addr - 0x20000000) or (addr & 0x0fffffff)
        v4l2_buf.addrVirY       =  (unsigned long)mPreviewWindow->ddr_vir[buf.index];
        v4l2_buf.width          = mFrameWidth;//mVideoInfo.nWidth;
        v4l2_buf.height         = mFrameHeight;//mVideoInfo.nHeight;
    }

    else
    {
        // may be (addr - 0x20000000) or (addr & 0x0fffffff)
        v4l2_buf.addrPhyY       = buf.m.offset;

#ifdef __SUN9I__
        v4l2_buf.addrPhyY       = buf.m.offset;
#endif
        v4l2_buf.addrVirY       = (unsigned long)mMapMem.mem[buf.index];
        v4l2_buf.width          = mFrameWidth;
        v4l2_buf.height         = mFrameHeight;
    }
    v4l2_buf.index              = buf.index;
    v4l2_buf.timeStamp          = mCurFrameTimestamp;

    v4l2_buf.crop_rect.left     = mRectCrop.left;
    v4l2_buf.crop_rect.top      = mRectCrop.top;
    v4l2_buf.crop_rect.width    = mRectCrop.right - mRectCrop.left + 1;
    v4l2_buf.crop_rect.height   = mRectCrop.bottom - mRectCrop.top + 1;
    v4l2_buf.format             = mVideoFormat;

    v4l2_buf.bytesused            = buf.bytesused;
    if (mHalCameraInfo.fast_picture_mode)
    {
        v4l2_buf.isThumbAvailable       = 1;
        v4l2_buf.thumbUsedForPreview    = 1;
        v4l2_buf.thumbUsedForPhoto      = 0;
        if(mIsThumbUsedForVideo == true)
        {
            v4l2_buf.thumbUsedForVideo  = 1;
        }
        else
        {
            v4l2_buf.thumbUsedForVideo  = 0;
        }
        v4l2_buf.thumbAddrPhyY          = v4l2_buf.addrPhyY +
                    ALIGN_4K(ALIGN_32B(mFrameWidth) * mFrameHeight * 3 / 2);    // to do
        v4l2_buf.thumbAddrVirY          = v4l2_buf.addrVirY +
                    ALIGN_4K(ALIGN_32B(mFrameWidth) * mFrameHeight * 3 / 2);    // to do
        v4l2_buf.thumbWidth             = mThumbWidth;
        v4l2_buf.thumbHeight            = mThumbHeight;
        v4l2_buf.thumb_crop_rect.left   = mThumbRectCrop.left;
        v4l2_buf.thumb_crop_rect.top    = mThumbRectCrop.top;
        v4l2_buf.thumb_crop_rect.width  = mThumbRectCrop.right - mThumbRectCrop.left;
        v4l2_buf.thumb_crop_rect.height = mThumbRectCrop.bottom - mThumbRectCrop.top;
        v4l2_buf.thumbFormat            = mVideoFormat;
    }
    else
    {
        v4l2_buf.isThumbAvailable        = 0;
    }
/*
    int SnrValue = getSnrValue();
    int SnrLevel = SnrValue >> 8;
    int Gain = (SnrValue & 0xff);

    if (SnrLevel != 0 && Gain > 16)
    {
        Gain = Gain/16;
        char *cnr_uv = (char *)v4l2_buf.thumbAddrVirY +
                        ALIGN_16B(v4l2_buf.thumbWidth) * v4l2_buf.thumbHeight;
        PreviewCnr(SnrLevel, Gain, v4l2_buf.thumbWidth, v4l2_buf.thumbHeight,cnr_uv , cnr_uv);
        if(mTakePictureState == TAKE_PICTURE_FAST)
        {
            cnr_uv = (char *)v4l2_buf.addrVirY + ALIGN_16B(mFrameWidth) * mFrameHeight;
            PreviewCnr(SnrLevel+1, Gain, mFrameWidth, mFrameHeight,cnr_uv , cnr_uv);
        }
    }
*/
    v4l2_buf.refCnt = 1;
    memcpy(&mV4l2buf[v4l2_buf.index], &v4l2_buf, sizeof(V4L2BUF_t));

    mV4l2buf[v4l2_buf.index].nShareBufFd = mPreviewWindow->mShareFd[v4l2_buf.index];

    if ((!mVideoHint) && (mTakePictureState != TAKE_PICTURE_NORMAL))
    {
        // face detection only use when picture mode
        mCurrentV4l2buf = &mV4l2buf[v4l2_buf.index];
    }

//TOOLS FOR CAPTURE BITSTREAM
//if we capture the stream that is H264,we must keep the first frame ,
//because the first frame of H264 stream own the sps(Sequence Parameter Sets) and pps(Picture Parameter Set) seri
#if DBG_CAPTURE_STREAM_FROM_V4L2DEVICE_FORH264
    if(frame_num0++ < LIMITED_NUM)
    {
        fwrite(mMapMem.mem[buf.index],buf.bytesused,1,fp_stream_from_v4l2device);
        fflush(fp_stream_from_v4l2device);
    }
#endif
#ifndef USE_CSI_VIN_DRIVER
    if(mDiscardFrameNum < DISCARD_FRAME_NUM) {
        mDiscardFrameNum ++;
        goto DEC_REF;
    } else if(mDiscardFrameNum < 100) {
        mDiscardFrameNum ++;
    } else {
        mDiscardFrameNum = DISCARD_FRAME_NUM + 1;
    }
#endif

//TOOLS FOR CAPTURE BITSTREAM
//We must be in there(after DISCARD_FRAME_NUM flag) ,because
//the first frame usb camera captured always is wrong..
#if DBG_CAPTURE_STREAM_FROM_V4L2DEVICE_FORMJPEG
    if(frame_num0++ < LIMITED_NUM)
    {
	fwrite(mMapMem.mem[buf.index],buf.bytesused,1,fp_stream_from_v4l2device);
	fflush(fp_stream_from_v4l2device);
	}
//The number of YUYV-bitstream we captured is 'mFrameWidth * mFrameHeight * 2'
//that(The number) is different from H264-bitstream and MJPEG-bitstream..
#elif DBG_CAPTURE_STREAM_FROM_V4L2DEVICE_FORYUYV
    if(frame_num0++ < LIMITED_NUM)
    {
        fwrite(mMapMem.mem[buf.index],mFrameWidth * mFrameHeight * 2,1,fp_stream_from_v4l2device);
        fflush(fp_stream_from_v4l2device);
    }
#elif DBG_CAPTURE_STREAM_FROM_V4L2DEVICE_FORNV21
    if(frame_num0++ < LIMITED_NUM)
    {
        fwrite(mMapMem.mem[buf.index],mFrameWidth * mFrameHeight * 3/2,1,fp_stream_from_v4l2device);
        fflush(fp_stream_from_v4l2device);
    }
#endif

#if DBG_CAPTURE_STREAM_AFTER_TRANSFORMATION
    //We only consider three formats(V4L2_PIX_FMT_YUYV
    //,V4L2_PIX_FMT_MJPEG,V4L2_PIX_FMT_H264).
    //Notice:
    //1.If mCaptureFormat == V4L2_PIX_FMT_NV21,
    //we cann't capture the bitstream in there.
    //2.@mFrameWidth * mFrameHeight *3/2 , NV12 has 12 bits == 1.5bytes ,
    //so , in there, we should write x*3/2==x*1.5 data to file.
    if(frame_num1++ < LIMITED_NUM)
    {
        fwrite((void*)mVideoBuffer.buf_vir_addr[buf.index],1,mFrameWidth * mFrameHeight *3/2,fp_stream_after_transformation);
        fflush(fp_stream_after_transformation);
    }
#endif


	if(mTakePictureState == TAKE_PICTURE_NORMAL)
	{
		LOGV("zjw,mTakePictureState == TAKE_PICTURE_NORMAL mPicBuffer.nShareBufFd = %d",mPicBuffer.nShareBufFd);
	}else if(mTakePictureState == TAKE_PICTURE_FAST)
	{
		LOGV("zjw,mTakePictureState == TAKE_PICTURE_FAST mV4l2buf[v4l2_buf.index].nShareBufFd = %d",mV4l2buf[v4l2_buf.index].nShareBufFd);
	}else if(mTakePictureState == TAKE_PICTURE_RECORD)
	{
		LOGV("zjw,mTakePictureState == TAKE_PICTURE_RECORD mV4l2buf[v4l2_buf.index].nShareBufFd = %d",mV4l2buf[v4l2_buf.index].nShareBufFd);
	}
	else if(mTakePictureState == TAKE_PICTURE_SMART)
	{
		LOGV("zjw,mTakePictureState == TAKE_PICTURE_SMART ");
	}
	else if(mTakePictureState == TAKE_PICTURE_CONTINUOUS)
	{
		LOGV("zjw,mTakePictureState == TAKE_PICTURE_CONTINUOUS ");
	}
	else if(mTakePictureState == TAKE_PICTURE_CONTINUOUS_FAST)
	{
		LOGV("zjw,mTakePictureState == TAKE_PICTURE_CONTINUOUS_FAST ");
	}

    if (mTakePictureState == TAKE_PICTURE_NORMAL)
    {
        //copy picture buffer
        unsigned long phy_addr = mPicBuffer.addrPhyY;
        unsigned long vir_addr = mPicBuffer.addrVirY;
        int nShareBufFd = mPicBuffer.nShareBufFd;

        int frame_size = ALIGN_16B(mFrameWidth) * ALIGN_16B(mFrameHeight) * 3 >> 1;


        if (frame_size > MAX_PICTURE_SIZE)
        {
            LOGE("picture buffer size(%d) is smaller than the frame buffer size(%d)",
                MAX_PICTURE_SIZE, frame_size);
            pthread_mutex_unlock(&mCaptureMutex);
            return false;
        }

        memcpy((void*)&mPicBuffer, &v4l2_buf, sizeof(V4L2BUF_t));
        mPicBuffer.addrPhyY = phy_addr;
        mPicBuffer.addrVirY = vir_addr;
        mPicBuffer.nShareBufFd = nShareBufFd;

        mMemOpsS->flush_cache_cam((void*)v4l2_buf.addrVirY, frame_size);
        memcpy((void*)mPicBuffer.addrVirY, (void*)v4l2_buf.addrVirY, mFrameWidth*mFrameHeight);
        memcpy((void*)(mPicBuffer.addrVirY+mFrameWidth *mFrameHeight),(void*)(v4l2_buf.addrVirY+ALIGN_16B(mFrameWidth) *ALIGN_16B(mFrameHeight)),mFrameWidth*mFrameHeight/2);
        mMemOpsS->flush_cache_cam((void*)mPicBuffer.addrVirY, frame_size);

#if DBG_CAPTURE_STREAM_FROM_V4L2DEVICE_TAKEPICTURE
fwrite((void*)mV4l2buf[v4l2_buf.index].addrVirY,ALIGN_16B(mFrameWidth) * ALIGN_16B(mFrameHeight) * 3/2,1,fp_stream_from_v4l2device);
fflush(fp_stream_from_v4l2device);
#endif

        // enqueue picture buffer
        ret = OSAL_Queue(&mQueueBufferPicture, &mPicBuffer);
        if (ret != 0)
        {
            LOGW("picture queue full");
            pthread_cond_signal(&mPictureCond);
            goto DEC_REF;
        }

        mIsPicCopy = true;
        mCaptureThreadState = CAPTURE_STATE_PAUSED;
        mTakePictureState = TAKE_PICTURE_NULL;
        pthread_cond_signal(&mPictureCond);

        goto DEC_REF;
    }
    else
    {
//??
#if  0 //USE_CSI_VIN_DRIVER //jiangwei
        mV4l2buf[v4l2_buf.index].nShareBufFd = mMapMem.nShareBufFd[v4l2_buf.index];
#endif
        ret = OSAL_Queue(&mQueueBufferPreview, &mV4l2buf[v4l2_buf.index]);
        if (ret != 0)
        {
            LOGW("preview queue full");
            goto DEC_REF;
        }

        // add reference count
        mV4l2buf[v4l2_buf.index].refCnt++;
        // signal a new frame for preview
        pthread_cond_signal(&mPreviewCond);

        if (mTakePictureState == TAKE_PICTURE_FAST
            || mTakePictureState == TAKE_PICTURE_RECORD)
        {
            if (mHalCameraInfo.fast_picture_mode)
            {
                if (buf.reserved == 0xFFFFFFFF)
                {
                    goto DEC_REF;
                }
            }

#if DBG_CAPTURE_STREAM_FROM_V4L2DEVICE_TAKEPICTURE
fwrite((void*)mV4l2buf[v4l2_buf.index].addrVirY,ALIGN_16B(mFrameWidth) * ALIGN_16B(mFrameHeight) * 3/2,1,fp_stream_from_v4l2device);
fflush(fp_stream_from_v4l2device);
#endif

            // enqueue picture buffer
            ret = OSAL_Queue(&mQueueBufferPicture, &mV4l2buf[v4l2_buf.index]);
            if (ret != 0)
            {
                LOGW("picture queue full");
                pthread_cond_signal(&mPictureCond);
                goto DEC_REF;
            }

            // add reference count
            mV4l2buf[v4l2_buf.index].refCnt++;
            mTakePictureState = TAKE_PICTURE_NULL;
            mIsPicCopy = false;
            pthread_cond_signal(&mPictureCond);
        }

        if (mTakePictureState == TAKE_PICTURE_SMART)
        {
            // enqueue picture buffer
            ret = OSAL_Queue(&mQueueBufferPicture, &mV4l2buf[v4l2_buf.index]);
            if (ret != 0)
            {
                LOGW("picture queue full");
                pthread_cond_signal(&mSmartPictureCond);
                goto DEC_REF;
            }

            // add reference count
            mV4l2buf[v4l2_buf.index].refCnt++;
            //mTakePictureState = TAKE_PICTURE_NULL;
            mIsPicCopy = false;
            pthread_cond_signal(&mSmartPictureCond);
        }

        if ((mTakePictureState == TAKE_PICTURE_CONTINUOUS
            || mTakePictureState == TAKE_PICTURE_CONTINUOUS_FAST)
            && isContinuousPictureTime())
        {
            // enqueue picture buffer
            ret = OSAL_Queue(&mQueueBufferPicture, &mV4l2buf[v4l2_buf.index]);
            if (ret != 0)
            {
                // LOGV("continuous picture queue full");
                pthread_cond_signal(&mContinuousPictureCond);
                goto DEC_REF;
            }

            // add reference count
            mV4l2buf[v4l2_buf.index].refCnt++;
            mIsPicCopy = false;
            pthread_cond_signal(&mContinuousPictureCond);
        }
    }

DEC_REF:
    pthread_mutex_unlock(&mCaptureMutex);

    releasePreviewFrame(v4l2_buf.index);

    return true;
}

bool V4L2CameraDevice::previewThread()
{
    V4L2BUF_t * pbuf = (V4L2BUF_t *)OSAL_Dequeue(&mQueueBufferPreview);
    int index_preview = -1;
    if (pbuf == NULL)
    {
        LOGV("previewThread queue no buffer, sleep...");
        pthread_mutex_lock(&mPreviewMutex);
        pthread_cond_wait(&mPreviewCond, &mPreviewMutex);
        pthread_mutex_unlock(&mPreviewMutex);
        return true;
    }

    Mutex::Autolock locker(&mObjectLock);
    if (mMapMem.mem[pbuf->index] == NULL
        /*|| pbuf->addrPhyY == 0*/)
    {
        LOGV("preview buffer have been released...");
        return true;
    }
    //showformat(pbuf->format,"CallBack");
    // callback
    if(mUseHwEncoder)
    {
        LOGV("zjw,mCallbackNotifier->onNextFrameAvailable pbuf->index = %d,",pbuf->index);
        mbufferUnit[pbuf->index].buffer_state = true;
        mbufferUnit[pbuf->index].mbufferUnitMetadata = (private_handle_t *)(*(mPreviewWindow->mBufferHandle[pbuf->index]));
    }

    mCallbackNotifier->onNextFrameAvailable(mPreviewWindow->mBufferHandle[pbuf->index],(void*)pbuf,mUseHwEncoder);


#ifdef __SUN9I__
    index_preview = mPreviewWindow->onNextFrameAvailable2(pbuf->index);
#else
    if (!mNeedHalfFrameRate || mShouldPreview)
    {
        index_preview = mPreviewWindow->onNextFrameAvailable2(pbuf->index);
        LOGV("zjw,mPreviewWindow->onNextFrameAvailable pbuf->index = %d,ret index_preview = %d\n",pbuf->index,index_preview);
    }else
    {
        index_preview = pbuf->index;
    }
    mShouldPreview = mShouldPreview ? false : true;
#endif

    // LOGD("preview id : %d", pbuf->index);
    //releasePreviewFrame(pbuf->index);

    releasePreviewFrame(index_preview);


    return true;
}

// singal picture
bool V4L2CameraDevice::pictureThread()
{
    V4L2BUF_t * pbuf = (V4L2BUF_t *)OSAL_Dequeue(&mQueueBufferPicture);
    if (pbuf == NULL)
    {
        LOGD("picture queue no buffer, sleep...");
        pthread_mutex_lock(&mPictureMutex);
        pthread_cond_wait(&mPictureCond, &mPictureMutex);
        pthread_mutex_unlock(&mPictureMutex);
        return true;
    }

    DBG_TIME_BEGIN("taking picture", 0);

    // notify picture cb
    mCameraHardware->notifyPictureMsg((void*)pbuf);

    DBG_TIME_DIFF("notifyPictureMsg");

    mCallbackNotifier->takePicture((void*)pbuf,(void *)mMemOpsS,false);

    char str[128];
    sprintf(str, "hw picture size: %dx%d", pbuf->width, pbuf->height);
    DBG_TIME_DIFF(str);

    if (!mIsPicCopy)
    {
        releasePreviewFrame(pbuf->index);
    }

    DBG_TIME_END("Take picture", 0);

    return true;
}

// blink picture
bool V4L2CameraDevice::smartPictureThread()
{
    V4L2BUF_t * pbuf = (V4L2BUF_t *)OSAL_Dequeue(&mQueueBufferPicture);

    if (pbuf == NULL)
    {
        LOGV("smartPictureThread queue no buffer, sleep...");
        pthread_mutex_lock(&mSmartPictureMutex);
        pthread_cond_wait(&mSmartPictureCond, &mSmartPictureMutex);
        pthread_mutex_unlock(&mSmartPictureMutex);
        return true;
    }

    // apk stop smart pictures
    if (mSmartPictureDone)
    {
        mTakePictureState = TAKE_PICTURE_NULL;
        if (!mIsPicCopy)
        {
            releasePreviewFrame(pbuf->index);
        }
        return true;
    }

    #if 1

    ALOGD("!! mCameraHardware->mBlinkPictureResult %d state %d",
    mCameraHardware->mBlinkPictureResult, mCameraHardware->mBlinkDetectionState);

    if ((mCameraHardware->mBlinkPictureResult == true) &&
        (mCameraHardware->mBlinkDetectionState == FACE_DETECTION_PREPARED))
    {
        DBG_TIME_BEGIN("taking blink picture", 0);

        // notify picture cb
        mCameraHardware->notifyPictureMsg((void*)pbuf);

        DBG_TIME_DIFF("notifyPictureMsg");

        mCallbackNotifier->takePicture((void*)pbuf,(void *)mMemOpsS,false);

        ALOGD("~~ smartPictureThread takePicture");

        stopSmartPicture();
        mTakePictureState = TAKE_PICTURE_NULL;
    }

    #endif

    #if 1

    ALOGD("!! mCameraHardware->mSmilePictureResult %d, state %d",
    mCameraHardware->mSmilePictureResult, mCameraHardware->mSmileDetectionState);

    if ((mCameraHardware->mSmilePictureResult == true) &&
        (mCameraHardware->mSmileDetectionState == FACE_DETECTION_PREPARED))
    {
        DBG_TIME_BEGIN("taking smile picture", 0);

        // notify picture cb
        mCameraHardware->notifyPictureMsg((void*)pbuf);

        DBG_TIME_DIFF("notifyPictureMsg");

        mCallbackNotifier->takePicture((void*)pbuf,(void *)mMemOpsS,false);

        ALOGD("~~ smartPictureThread takePicture");

        stopSmartPicture();
        mTakePictureState = TAKE_PICTURE_NULL;
    }

    #endif

    char str[128];
    sprintf(str, "hw picture size: %dx%d", pbuf->width, pbuf->height);
    DBG_TIME_DIFF(str);

    if (!mIsPicCopy)
    {
        releasePreviewFrame(pbuf->index);
    }

    DBG_TIME_END("Take smart picture", 0);


    return true;
}

void V4L2CameraDevice::startSmartPicture()
{
    F_LOG;

    mSmartPictureDone = false;

    DBG_TIME_AVG_INIT(TAG_SMART_PICTURE);
}

void V4L2CameraDevice::stopSmartPicture()
{
    F_LOG;

    if (mSmartPictureDone)
    {
        LOGD("Smart picture has already stopped");
        return;
    }

    mSmartPictureDone = true;

    DBG_TIME_AVG_END(TAG_SMART_PICTURE, "picture enc");
}


// continuous picture
bool V4L2CameraDevice::continuousPictureThread()
{
    V4L2BUF_t * pbuf = (V4L2BUF_t *)OSAL_Dequeue(&mQueueBufferPicture);
    if (pbuf == NULL)
    {
        LOGV("continuousPictureThread queue no buffer, sleep...");
        pthread_mutex_lock(&mContinuousPictureMutex);
        pthread_cond_wait(&mContinuousPictureCond, &mContinuousPictureMutex);
        pthread_mutex_unlock(&mContinuousPictureMutex);
        return true;
    }

    Mutex::Autolock locker(&mObjectLock);
    if (mMapMem.mem[pbuf->index] == NULL
        || pbuf->addrPhyY == 0)
    {
        LOGV("picture buffer have been released...");
        return true;
    }

    DBG_TIME_AVG_AREA_IN(TAG_CONTINUOUS_PICTURE);

    // reach the max number of pictures
    if (mContinuousPictureCnt >= mContinuousPictureMax)
    {
        mTakePictureState = TAKE_PICTURE_NULL;
        stopContinuousPicture();
        releasePreviewFrame(pbuf->index);
        return true;
    }

    // apk stop continuous pictures
    if (!mContinuousPictureStarted)
    {
        mTakePictureState = TAKE_PICTURE_NULL;
        releasePreviewFrame(pbuf->index);
        return true;
    }

    bool ret = mCallbackNotifier->takePicture((void*)pbuf, (void *)mMemOpsS,true);
    if (ret)
    {
        mContinuousPictureCnt++;

        DBG_TIME_AVG_AREA_OUT(TAG_CONTINUOUS_PICTURE);
    }
    else
    {
        // LOGW("do not encoder jpeg");
    }

    releasePreviewFrame(pbuf->index);

    return true;
}

void V4L2CameraDevice::startContinuousPicture()
{
    F_LOG;

    mContinuousPictureCnt = 0;
    mContinuousPictureStarted = true;
    mContinuousPictureStartTime = systemTime(SYSTEM_TIME_MONOTONIC);

    DBG_TIME_AVG_INIT(TAG_CONTINUOUS_PICTURE);
}

void V4L2CameraDevice::stopContinuousPicture()
{
    F_LOG;

    if (!mContinuousPictureStarted)
    {
        LOGD("Continuous picture has already stopped");
        return;
    }

    mContinuousPictureStarted = false;

    nsecs_t time = (systemTime(SYSTEM_TIME_MONOTONIC) - mContinuousPictureStartTime)/1000000;
    LOGD("Continuous picture cnt: %d, use time %lld(ms)", mContinuousPictureCnt, time);
    if (time != 0)
    {
        LOGD("Continuous picture %f(fps)", (float)mContinuousPictureCnt/(float)time * 1000);
    }

    DBG_TIME_AVG_END(TAG_CONTINUOUS_PICTURE, "picture enc");
}

void V4L2CameraDevice::setContinuousPictureCnt(int cnt)
{
    F_LOG;
    mContinuousPictureMax = cnt;
}

bool V4L2CameraDevice::isContinuousPictureTime()
{
    if (mTakePictureState == TAKE_PICTURE_CONTINUOUS_FAST)
    {
        return true;
    }

    timeval cur_time;
    gettimeofday(&cur_time, NULL);
    const uint64_t cur_mks = cur_time.tv_sec * 1000000LL + cur_time.tv_usec;
    if ((cur_mks - mContinuousPictureLast) >= mContinuousPictureAfter) {
        mContinuousPictureLast = cur_mks;
        return true;
    }
    return false;
}

bool V4L2CameraDevice::isPreviewTime()
{
    if (mVideoHint != true)
    {
        return true;
    }

    timeval cur_time;
    gettimeofday(&cur_time, NULL);
    const uint64_t cur_mks = cur_time.tv_sec * 1000000LL + cur_time.tv_usec;
    if ((cur_mks - mPreviewLast) >= mPreviewAfter) {
        mPreviewLast = cur_mks;
        return true;
    }
    return false;
}

void V4L2CameraDevice::waitFaceDectectTime()
{
    timeval cur_time;
    gettimeofday(&cur_time, NULL);
    const uint64_t cur_mks = cur_time.tv_sec * 1000000LL + cur_time.tv_usec;

    if ((cur_mks - mFaceDectectLast) >= mFaceDectectAfter)
    {
        mFaceDectectLast = cur_mks;
    }
    else
    {
        usleep(mFaceDectectAfter - (cur_mks - mFaceDectectLast));
        gettimeofday(&cur_time, NULL);
        mFaceDectectLast = cur_time.tv_sec * 1000000LL + cur_time.tv_usec;
    }
}

int V4L2CameraDevice::getCurrentFaceFrame(void * frame)
{
    int len = 0;
    if (frame == NULL)
    {
        LOGE("getCurrentFrame: error in null pointer");
        return -1;
    }

    pthread_mutex_lock(&mCaptureMutex);
    // stop capture
    if (mCaptureThreadState != CAPTURE_STATE_STARTED)
    {
        LOGW("capture thread dose not started");
        pthread_mutex_unlock(&mCaptureMutex);
        return -1;
    }
    pthread_mutex_unlock(&mCaptureMutex);

    //waitFaceDectectTime();

    Mutex::Autolock locker(&mObjectLock);

    if (mCurrentV4l2buf == NULL
        || mCurrentV4l2buf->addrVirY == 0)
    {
        LOGW("frame buffer not ready");
        return -1;
    }

    if ((mCurrentV4l2buf->isThumbAvailable == 1)
        && (mCurrentV4l2buf->thumbUsedForPreview == 1))
    {
        memcpy(frame,
               (void*)(mCurrentV4l2buf->addrVirY +
                   ALIGN_4K(ALIGN_16B(mCurrentV4l2buf->width) * mCurrentV4l2buf->height * 3 / 2)),
               ALIGN_16B(mCurrentV4l2buf->thumbWidth) * mCurrentV4l2buf->thumbHeight);

        len = ALIGN_16B(mCurrentV4l2buf->thumbWidth) * mCurrentV4l2buf->thumbHeight;
    }
    else
    {
        memcpy(frame,
               (void*)mCurrentV4l2buf->addrVirY,
               mCurrentV4l2buf->width * mCurrentV4l2buf->height);
        len =  mCurrentV4l2buf->width * mCurrentV4l2buf->height;
    }

    //return 0;
    return len;
}

// -----------------------------------------------------------------------------
// extended interfaces here <***** star *****>
// -----------------------------------------------------------------------------
int V4L2CameraDevice::openCameraDev(HALCameraInfo * halInfo)
{
    F_LOG;

    int ret = -1;
    struct v4l2_input inp;
    struct v4l2_capability cap;
    char dev_node[16];

    if (halInfo == NULL)
    {
        LOGE("error HAL camera info");
        return -1;
    }

    // open V4L2 device
    //-----------------------------------------------
    //If "video0" not exist, use others instead
    //Modified by Microphone
    //2013-11-14
    //-----------------------------------------------
    if((access(halInfo->device_name, F_OK)) == 0)
    {
        strcpy(dev_node,halInfo->device_name);
    }
    else
    {
        for (int i = 0; i < MAX_NUM_OF_CAMERAS; i++)
        {
            sprintf(dev_node, "/dev/video%d", i);
            ret = access(dev_node, F_OK);
            if(ret == 0)
            {
                break;
            }
        }
    }
    mCameraFd = open(dev_node, O_RDWR | O_NONBLOCK, 0);
    if (mCameraFd == -1)
    {
        LOGE("ERROR opening %s: %s", dev_node, strerror(errno));
        return -1;
    }
    //-------------------------------------------------
    // check v4l2 device capabilities
    ret = ioctl (mCameraFd, VIDIOC_QUERYCAP, &cap);
    if (ret < 0)
    {
        LOGE("Error opening device: unable to query device.");
        goto END_ERROR;
    }

    if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == 0)
    {
        LOGE("Error opening device: video capture not supported.");
        goto END_ERROR;
    }

    if ((cap.capabilities & V4L2_CAP_STREAMING) == 0)
    {
        LOGE("Capture device does not support streaming i/o");
        goto END_ERROR;
    }

    if (!strcmp((char *)cap.driver, "uvcvideo"))
    {
        mIsUsbCamera = true;
    }
    LOGD("The name of the Camera is '%s'",cap.card);

    if (!mIsUsbCamera)
    {
        // uvc do not need to set input
        inp.index = halInfo->device_id;
        if (-1 == ioctl (mCameraFd, VIDIOC_S_INPUT, &inp))
        {
            LOGE("VIDIOC_S_INPUT error!");
            goto END_ERROR;
        }
    }

    if (mIsUsbCamera)
    {
        // try to support this format: NV21, YUYV
        // we do not support mjpeg camera now

        if (tryFmt(V4L2_PIX_FMT_NV21) == OK)
        {
            mCaptureFormat = V4L2_PIX_FMT_NV21;
            LOGV("capture format: V4L2_PIX_FMT_NV21");
        }
        else if(tryFmt(V4L2_PIX_FMT_MJPEG) == OK)
        {
            mCaptureFormat = V4L2_PIX_FMT_MJPEG;        // maybe usb camera
            LOGD("MJPEG Camera '%s' is Supported",cap.card);
            LOGV("capture format: V4L2_PIX_FMT_MJPEG");
        }
        else if(tryFmt(V4L2_PIX_FMT_YUYV) == OK)
        {
            mCaptureFormat = V4L2_PIX_FMT_YUYV;        // maybe usb camera
            LOGV("capture format: V4L2_PIX_FMT_YUYV");
        }
        else if(tryFmt(V4L2_PIX_FMT_H264) == OK)
        {
            mCaptureFormat = V4L2_PIX_FMT_H264;
            LOGV("capture format: V4L2_PIX_FMT_H264");
        }
        else
        {
            LOGE("driver should surpport NV21/NV12 or YUYV format, but it not!");
            goto END_ERROR;
        }
    }

    return OK;

END_ERROR:

    if (mCameraFd != (int)NULL)
    {
        close(mCameraFd);
        mCameraFd = (int)NULL;
    }

    return -1;
}

void V4L2CameraDevice::closeCameraDev()
{
    F_LOG;

    if (mCameraFd != (int)NULL)
    {
        close(mCameraFd);
        mCameraFd = (int)NULL;
    }
}

int V4L2CameraDevice::v4l2SetVideoParams(int width, int height, uint32_t pix_fmt)
{
    int ret = UNKNOWN_ERROR;
    struct v4l2_format format;

    LOGV("%s, line: %d, w: %d, h: %d, pfmt: %d",
        __FUNCTION__, __LINE__, width, height, pix_fmt);

    memset(&format, 0, sizeof(format));

    if (mHalCameraInfo.is_uvc)
    {
        format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        format.fmt.pix.width  = width;
        format.fmt.pix.height = height;
    }else
    {
#if USE_CSI_VIN_DRIVER //jiangwei
        format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        format.fmt.pix_mp.width  = width;
        format.fmt.pix_mp.height = height;
#else
        format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        format.fmt.pix.width  = width;
        format.fmt.pix.height = height;
#endif
    }

    if(mCaptureFormat == V4L2_PIX_FMT_MJPEG)
    {
        format.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
        LOGV("set capture format: V4L2_PIX_FMT_MJPEG");
    }
    else if (mCaptureFormat == V4L2_PIX_FMT_YUYV)
    {
        format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
        LOGV("set capture format: V4L2_PIX_FMT_YUYV");
    }
    else if (mCaptureFormat == V4L2_PIX_FMT_H264)
    {
        format.fmt.pix.pixelformat = V4L2_PIX_FMT_H264;
        LOGV("set capture format: V4L2_PIX_FMT_YUYV");
    }
    else
    {

        if (mHalCameraInfo.is_uvc)
        {
            format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            format.fmt.pix.width  = width;
            format.fmt.pix.height = height;
        }else
        {
#if USE_CSI_VIN_DRIVER //jiangwei
            format.fmt.pix_mp.pixelformat = pix_fmt;
#else
            format.fmt.pix.pixelformat = pix_fmt;
#endif
        }

    }

    if (mHalCameraInfo.is_uvc)
    {
        format.fmt.pix.field = V4L2_FIELD_NONE;
    }else
    {
#if USE_CSI_VIN_DRIVER //jiangwei
        format.fmt.pix_mp.field = V4L2_FIELD_NONE;
#else
        format.fmt.pix.field = V4L2_FIELD_NONE;
#endif
    }


    struct v4l2_pix_format sub_fmt;
    if (mHalCameraInfo.fast_picture_mode)
    {
        memset(&sub_fmt, 0, sizeof(sub_fmt));

        int scale = getSuitableThumbScale(width, height);
        if (scale <= 0)
        {
            LOGE("error thumb scale: %d, src_size: %dx%d", scale, width, height);
            return UNKNOWN_ERROR;
        }

        format.fmt.pix.subchannel = &sub_fmt;
        format.fmt.pix.subchannel->width = format.fmt.pix.width / scale;
        format.fmt.pix.subchannel->height = format.fmt.pix.height / scale;
        format.fmt.pix.subchannel->pixelformat = pix_fmt;
        format.fmt.pix.subchannel->field = V4L2_FIELD_NONE;

        if (mHalCameraInfo.is_uvc)
        {
            LOGV("to camera params: w: %d, h: %d, sub: %dx%d, pfmt: %d, pfield: %d",
                format.fmt.pix.width,
                format.fmt.pix.height,
                format.fmt.pix.subchannel->width,
                format.fmt.pix.subchannel->height,
                pix_fmt,
                V4L2_FIELD_NONE);
        }else
        {
        #if USE_CSI_VIN_DRIVER //jiangwei
               LOGV("to camera params: w: %d, h: %d, sub: %dx%d, pfmt: %d, pfield: %d",
                   format.fmt.pix_mp.width,
                   format.fmt.pix_mp.height,
                   format.fmt.pix.subchannel->width,
                   format.fmt.pix.subchannel->height,
                   pix_fmt,
                   V4L2_FIELD_NONE);
       #else
               LOGV("to camera params: w: %d, h: %d, sub: %dx%d, pfmt: %d, pfield: %d",
                   format.fmt.pix.width,
                   format.fmt.pix.height,
                   format.fmt.pix.subchannel->width,
                   format.fmt.pix.subchannel->height,
                   pix_fmt,
                   V4L2_FIELD_NONE);
       #endif
        }

    }

    ret = ioctl(mCameraFd, VIDIOC_S_FMT, &format);
    if (ret < 0)
    {
        LOGE("VIDIOC_S_FMT Failed: %s", strerror(errno));
        return ret;
    }

    if (mHalCameraInfo.is_uvc)
    {
        mFrameWidth = format.fmt.pix.width;
        mFrameHeight = format.fmt.pix.height;
    }else
    {
    #if USE_CSI_VIN_DRIVER //jiangwei
        ret = ioctl(mCameraFd, VIDIOC_G_FMT, &format);
        if (ret < 0 )
        {
             LOGD("VIDIOC_G_FMT Failed: %s", strerror(errno));
             return ret;
        } else
        {
            nplanes = format.fmt.pix_mp.num_planes;
            LOGD("resolution got from sensor = %d*%d num_planes = %d\n",format.fmt.pix_mp.width, format.fmt.pix_mp.height,format.fmt.pix_mp.num_planes);
        }
        mFrameWidth = format.fmt.pix_mp.width;
        mFrameHeight = format.fmt.pix_mp.height;
    #else
        mFrameWidth = format.fmt.pix.width;
        mFrameHeight = format.fmt.pix.height;
    #endif
    }

    if (mHalCameraInfo.fast_picture_mode)
    {
        mThumbWidth = format.fmt.pix.subchannel->width;
        mThumbHeight = format.fmt.pix.subchannel->height;
    }

    LOGV("camera params: w: %d, h: %d, sub: %dx%d, pfmt: %d, pfield: %d",
        mFrameWidth, mFrameHeight, mThumbWidth, mThumbHeight, pix_fmt, V4L2_FIELD_NONE);

    return OK;
}

int V4L2CameraDevice::v4l2ReqBufs(int * buf_cnt)
{
    F_LOG;
    int ret = UNKNOWN_ERROR;
    struct v4l2_requestbuffers rb;

    LOGV("TO VIDIOC_REQBUFS count: %d", *buf_cnt);

    memset(&rb, 0, sizeof(rb));
    if (mHalCameraInfo.is_uvc)
    {
        rb.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        rb.memory = V4L2_MEMORY_MMAP;
    }else
    {
        #if USE_CSI_VIN_DRIVER //jiangwei
        rb.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
#if 1 //jiangwei
        rb.memory = V4L2_MEMORY_USERPTR;
#endif
        #else
        rb.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        rb.memory = V4L2_MEMORY_MMAP;
        #endif
    }

    rb.count  = *buf_cnt;

    ret = ioctl(mCameraFd, VIDIOC_REQBUFS, &rb);
    if (ret < 0)
    {
        LOGE("Init: VIDIOC_REQBUFS failed: %s", strerror(errno));
        return ret;
    }

    *buf_cnt = rb.count;
    LOGV("VIDIOC_REQBUFS count: %d", *buf_cnt);

    return OK;
}

int V4L2CameraDevice::v4l2QueryBuf()
{
    F_LOG;
    int ret = UNKNOWN_ERROR;
    struct v4l2_buffer buf;
    struct v4l2_plane planes[VIDEO_MAX_PLANES];
    memset(planes, 0, VIDEO_MAX_PLANES*sizeof(struct v4l2_plane));
	

    for (int i = 0; i < mBufferCnt; i++)
    {
        memset (&buf, 0, sizeof (struct v4l2_buffer));
        if (mHalCameraInfo.is_uvc)
        {
            buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index  = i;
        }else
        {
            #if USE_CSI_VIN_DRIVER //jiangwei
                buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
                buf.memory = V4L2_MEMORY_USERPTR;
                buf.length = nplanes;
                buf.m.planes =planes;
                buf.index  = i;
                if (NULL == buf.m.planes) {
                    LOGE("buf.m.planes calloc failed!\n");
                }
            #else
                buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;
                buf.index  = i;
            #endif
        }

        ret = ioctl (mCameraFd, VIDIOC_QUERYBUF, &buf);
        if (ret < 0)
        {
            LOGE("Unable to query buffer (%s)", strerror(errno));
            return ret;
        }

        switch (buf.memory) {
            case V4L2_MEMORY_MMAP:
            {
                mMapMem.mem[i] = mmap (0, buf.length,
                                                           PROT_READ | PROT_WRITE,
                                                           MAP_SHARED,
                                                           mCameraFd,
                                                           buf.m.offset);
                mMapMem.length = buf.length;
                LOGD("index: %d, mem: %x, len: %x, offset: %x",
                            i, (unsigned long)mMapMem.mem[i], buf.length, buf.m.offset);
                if (mMapMem.mem[i] == MAP_FAILED)
                {
                    LOGE("Unable to map buffer (%s)", strerror(errno));
                    munmap(mMapMem.mem[i], mMapMem.length);
                    return -1;
                }
                break;
            };
            case V4L2_MEMORY_USERPTR:
            {
                /* by zjw */
                mMapMem.mem[i] = (void*)mPreviewWindow->ddr_vir[i];
                lengthY = buf.m.planes[0].length;
                lengthUV = buf.m.planes[1].length;

                if (mMapMem.mem[i] == NULL)
                {
                     LOGE("zjw,mMapMem.mem[i] == NULL get addr_vir ERROR (%s)", strerror(errno));
                     return -1;
                }

                buf.m.planes[0].m.userptr = (unsigned long)mMapMem.mem[i];
                buf.m.planes[1].m.userptr = (unsigned long)mMapMem.mem[i]+ALIGN_16B(mFrameWidth)*ALIGN_16B(mFrameHeight);
                buf.m.planes[0].length = lengthY;
                buf.m.planes[1].length = lengthUV;				
                mMapMem.length = lengthY +lengthUV;
                LOGD("jiangwei,index: %d, mem: %x, len: %d, lengthY: %d,lengthUV:%d",
                   i, (unsigned long)mMapMem.mem[i], mMapMem.length,lengthY,lengthUV);

                break;
            };
            default:
                break;

        }


#if 0
        if (mMapMem.mem[i] == MAP_FAILED)
        {
            LOGE("Unable to map buffer (%s)", strerror(errno));
            for(int j = 0;j < i;j++){
                munmap(mMapMem.mem[j], mMapMem.length);
            }
            return -1;
        }
#endif

        // start with all buffers in queue
        ret = ioctl(mCameraFd, VIDIOC_QBUF, &buf);
        if (ret < 0)
        {
            LOGE("VIDIOC_QBUF Failed  ERROR (%s) ", strerror(ret));
            return ret;
        }

        if (mIsUsbCamera)        // star to do
        {
            int buffer_len = mFrameWidth * mFrameHeight * 2;
        
            //mVideoBuffer.buf_vir_addr[i] = (unsigned long)mMemOpsS->palloc_cam(buffer_len,SHARE_BUFFER_FD,&(mVideoBuffer.nShareBufFd[i]));;

            //LOGV("video buffer: index: %d, vir: %x, len: %x", 
             //       i, mVideoBuffer.buf_vir_addr[i], buffer_len);

            //memset((void*)mVideoBuffer.buf_vir_addr[i], 0x10, mFrameWidth * mFrameHeight);
            //memset((void*)(mVideoBuffer.buf_vir_addr[i] + mFrameWidth * mFrameHeight),
            //        0x80, mFrameWidth * mFrameHeight / 2);
        }
    }

    return OK;
}

int V4L2CameraDevice::v4l2StartStreaming()
{
    F_LOG;
    int ret = UNKNOWN_ERROR;
    enum v4l2_buf_type type;


    if (mHalCameraInfo.is_uvc)
    {
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    }else
    {
        #if USE_CSI_VIN_DRIVER //jiangwei
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        #else
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        #endif
    }

      ret = ioctl (mCameraFd, VIDIOC_STREAMON, &type);
    if (ret < 0)
    {
        LOGE("StartStreaming: Unable to start capture: %s", strerror(errno));
        return ret;
    }

    return OK;
}

int V4L2CameraDevice::v4l2StopStreaming()
{
    F_LOG;
    int ret = UNKNOWN_ERROR;
    enum v4l2_buf_type type;

    if (mHalCameraInfo.is_uvc)
    {
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    }else
    {
        #if USE_CSI_VIN_DRIVER //jiangwei
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        #else
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        #endif
    }

    ret = ioctl (mCameraFd, VIDIOC_STREAMOFF, &type);
    if (ret < 0)
    {
        LOGE("StopStreaming: Unable to stop capture: %s", strerror(errno));
        return ret;
    }
    LOGV("V4L2Camera::v4l2StopStreaming OK");

    return OK;
}

int V4L2CameraDevice::v4l2UnmapBuf()
{
    F_LOG;
    int ret = UNKNOWN_ERROR;

#ifdef USE_CSI_VIN_DRIVER
    for (int i = 0; i < mBufferCnt; i++)
    {
        mMapMem.mem[i] = NULL;
        mPreviewWindow->unlockBuffer(mFrameWidth,mFrameHeight,i);
    }
#else
    for (int i = 0; i < mBufferCnt; i++)
    {
        ret = munmap(mMapMem.mem[i], mMapMem.length);
        if (ret < 0)
        {
            LOGE("v4l2CloseBuf Unmap failed");
            return ret;
        }

        mMapMem.mem[i] = NULL;

        if (mVideoBuffer.buf_vir_addr[i] != 0)
        {
            //mMemOpsS->pfree((void*)mVideoBuffer.buf_vir_addr[i]);
            //mVideoBuffer.buf_phy_addr[i] = 0;
        }
    }
#endif
    mVideoBuffer.buf_unused = NB_BUFFER;
    mVideoBuffer.read_id = 0;
    mVideoBuffer.read_id = 0;

    return OK;
}

void V4L2CameraDevice::releasePreviewFrame(int index)
{
    if (index > NB_BUFFER)
    {
        LOGE("error, index wrong, F:%s,L:%d",__FUNCTION__,__LINE__);
        return;
    }else if(index < 0)
    {
        LOGW("zjw, index < 0, F:%s,L:%d",__FUNCTION__,__LINE__);
        return;
    }
    int ret = UNKNOWN_ERROR;
    struct v4l2_buffer buf;
    struct v4l2_plane planes[VIDEO_MAX_PLANES];

    pthread_mutex_lock(&mCaptureMutex);

    LOGV("zjw,call releasePreviewFrame mV4l2buf[index].refCnt = %d,index = %d,\n",mV4l2buf[index].refCnt,index);

    // decrease buffer reference count first, if the reference count is no more than 0, release it.
    if (mV4l2buf[index].refCnt > 0
        && --mV4l2buf[index].refCnt == 0)
    {

        memset(&buf, 0, sizeof(v4l2_buffer));
        if (mHalCameraInfo.is_uvc)
        {
               buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
               buf.memory = V4L2_MEMORY_MMAP;

        }else
        {
        #if USE_CSI_VIN_DRIVER //jiangwei
               buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
               buf.length = nplanes;
               buf.m.planes = planes;
               buf.memory = V4L2_MEMORY_USERPTR;

		buf.m.planes[0].m.userptr = (unsigned long)mMapMem.mem[index];
		buf.m.planes[1].m.userptr = (unsigned long)mMapMem.mem[index]+ALIGN_16B(mFrameWidth)*ALIGN_16B(mFrameHeight);
		buf.m.planes[0].length = lengthY;
		buf.m.planes[1].length = lengthUV;
		mMapMem.length = lengthY +lengthUV;

        #else
               buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
               buf.memory = V4L2_MEMORY_MMAP;

        #endif
        }

        buf.index = index;

        LOGV("zjw,V4L2CameraDevice::releasePreviewFrame index = %d,\n",index);

        // LOGD("r ID: %d", buf.index);
        ret = ioctl(mCameraFd, VIDIOC_QBUF, &buf);
        if (ret != 0)
        {
            LOGE("releasePreviewFrame: VIDIOC_QBUF Failed: index = %d, ret = %d, %s",
                buf.index, ret, strerror(errno));
        }
        else
        {
            mCurAvailBufferCnt++;
        }
    }

    pthread_mutex_unlock(&mCaptureMutex);
}

int V4L2CameraDevice::getPreviewFrame(v4l2_buffer *buf)
{
    int ret = UNKNOWN_ERROR;
    buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf->memory = V4L2_MEMORY_MMAP;

    struct v4l2_plane planes[VIDEO_MAX_PLANES];

    if (mHalCameraInfo.is_uvc)
    {
        buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf->memory = V4L2_MEMORY_MMAP;
    }else
    {
        #if USE_CSI_VIN_DRIVER //jiangwei
        buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
#if 1
        buf->memory = V4L2_MEMORY_USERPTR;
#endif
        buf->length = nplanes;
        buf->m.planes =planes;
        #else
        buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf->memory = V4L2_MEMORY_MMAP;
        #endif
    }


    ret = ioctl(mCameraFd, VIDIOC_DQBUF, buf);
    if (ret < 0)
    {
        LOGW("GetPreviewFrame: VIDIOC_DQBUF Failed, %s", strerror(errno));
        return __LINE__;             // can not return false
    }

    return OK;
}

int V4L2CameraDevice::tryFmt(int format)
{
    struct v4l2_fmtdesc fmtdesc;

    if (mHalCameraInfo.is_uvc)
    {
        fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    }else
    {
        #if USE_CSI_VIN_DRIVER //jiangwei
        fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        #else
        fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        #endif
    }

    for(int i = 0; i < 12; i++)
    {
        fmtdesc.index = i;
        if (-1 == ioctl (mCameraFd, VIDIOC_ENUM_FMT, &fmtdesc))
        {
            break;
        }
        LOGV("format index = %d, name = %s, v4l2 pixel format = %x\n",
            i, fmtdesc.description, fmtdesc.pixelformat);

        if (fmtdesc.pixelformat == (unsigned int)format)
        {
            return OK;
        }
    }

    return -1;
}

int V4L2CameraDevice::tryFmtSize(int * width, int * height)
{
    F_LOG;
    int ret = -1;
    struct v4l2_format fmt;

    LOGV("V4L2Camera::TryFmtSize: w: %d, h: %d", *width, *height);


    memset(&fmt, 0, sizeof(fmt));

    if (mHalCameraInfo.is_uvc)
    {
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    }else
    {
        #if USE_CSI_VIN_DRIVER //jiangwei
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        fmt.fmt.pix_mp.pixelformat = mVideoFormat;
        fmt.fmt.pix_mp.width  = *width;
        fmt.fmt.pix_mp.height = *height;
        fmt.fmt.pix_mp.field = V4L2_FIELD_NONE;
        #else
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        #endif
    }

    fmt.fmt.pix.width  = *width;
    fmt.fmt.pix.height = *height;
    if(mCaptureFormat == V4L2_PIX_FMT_MJPEG)
    {
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    }
    else if (mCaptureFormat == V4L2_PIX_FMT_YUYV)
    {
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    }
    else if (mCaptureFormat == V4L2_PIX_FMT_H264)
    {
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_H264;
    }
    else
    {
        fmt.fmt.pix.pixelformat = mVideoFormat;
    }
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    ret = ioctl(mCameraFd, VIDIOC_TRY_FMT, &fmt);
    if (ret < 0)
    {
        LOGE("VIDIOC_TRY_FMT Failed: %s", strerror(errno));
        return ret;
    }

    // driver surpport this size
    *width = fmt.fmt.pix.width;
    *height = fmt.fmt.pix.height;
#if USE_CSI_VIN_DRIVER //jiangwei
    *width = fmt.fmt.pix_mp.width;
    *height = fmt.fmt.pix_mp.height;
#endif

    return 0;
}

int V4L2CameraDevice::setFrameRate(int rate)
{
    mFrameRate = rate;
    return OK;
}

int V4L2CameraDevice::getFrameRate()
{
    F_LOG;
    int ret = -1;

    struct v4l2_streamparm parms;
    if (mHalCameraInfo.is_uvc)
    {
        parms.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    }else
    {
        #if USE_CSI_VIN_DRIVER //jiangwei
        parms.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        #else
        parms.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        #endif
    }

    ret = ioctl (mCameraFd, VIDIOC_G_PARM, &parms);
    if (ret < 0)
    {
        LOGE("VIDIOC_G_PARM getFrameRate error, %s", strerror(errno));
        return ret;
    }

    int numerator = parms.parm.capture.timeperframe.numerator;
    int denominator = parms.parm.capture.timeperframe.denominator;

    LOGV("frame rate: numerator = %d, denominator = %d", numerator, denominator);

    if (numerator != 0
        && denominator != 0)
    {
        return denominator / numerator;
    }
    else
    {
        LOGW("unsupported frame rate: %d/%d", denominator, numerator);
        return 30;
    }
}

int V4L2CameraDevice::setImageEffect(int effect)
{
    F_LOG;
    int ret = -1;
    struct v4l2_control ctrl;

    ctrl.id = V4L2_CID_COLORFX;
    ctrl.value = effect;
    ret = ioctl(mCameraFd, VIDIOC_S_CTRL, &ctrl);
    if (ret < 0)
        LOGV("setImageEffect failed!");
    else
        LOGV("setImageEffect ok");

    return ret;
}

int V4L2CameraDevice::setWhiteBalance(int wb)
{
    struct v4l2_control ctrl;
    int ret = -1;

    ctrl.id = V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE;
    ctrl.value = wb;
    ret = ioctl(mCameraFd, VIDIOC_S_CTRL, &ctrl);
    if (ret < 0)
        LOGV("setWhiteBalance failed, %s", strerror(errno));
    else
        LOGV("setWhiteBalance ok");

    return ret;
}

int V4L2CameraDevice::setTakePictureCtrl()
{
    struct v4l2_control ctrl;
    int ret = -1;
    if (mHalCameraInfo.fast_picture_mode){
        ctrl.id = V4L2_CID_TAKE_PICTURE;
        ret = ioctl(mCameraFd, VIDIOC_S_CTRL, &ctrl);
        if (ret < 0)
            LOGV("setTakePictureCtrl failed, %s", strerror(errno));
        else
            LOGV("setTakePictureCtrl ok");

        return ret;
    }
    return 0;
}

// ae mode
int V4L2CameraDevice::setExposureMode(int mode)
{
    F_LOG;
    int ret = -1;
    struct v4l2_control ctrl;

    ctrl.id = V4L2_CID_EXPOSURE_AUTO;
    ctrl.value = mode;
    ret = ioctl(mCameraFd, VIDIOC_S_CTRL, &ctrl);
    if (ret < 0)
        LOGV("setExposureMode failed, %s", strerror(errno));
    else
        LOGV("setExposureMode ok");

    return ret;
}

// ae compensation
int V4L2CameraDevice::setExposureCompensation(int val)
{
    F_LOG;
    int ret = -1;
    struct v4l2_control ctrl;

    ctrl.id = V4L2_CID_AUTO_EXPOSURE_BIAS;
    ctrl.value = val;
    ret = ioctl(mCameraFd, VIDIOC_S_CTRL, &ctrl);
    if (ret < 0)
        LOGV("setExposureCompensation failed, %s", strerror(errno));
    else
        LOGV("setExposureCompensation ok");

    return ret;
}

// ae compensation
int V4L2CameraDevice::setExposureWind(int num, void *wind)
{
    F_LOG;
    int ret = -1;
    struct v4l2_control ctrl;

    ctrl.id = V4L2_CID_AUTO_EXPOSURE_WIN_NUM;
    ctrl.value = num;
    ctrl.user_pt = (unsigned int)wind;
    ret = ioctl(mCameraFd, VIDIOC_S_CTRL, &ctrl);
    if (ret < 0)
        LOGE("setExposureWind failed, %s", strerror(errno));
    else
        LOGV("setExposureWind ok");

    return ret;
}

// flash mode
int V4L2CameraDevice::setFlashMode(int mode)
{
    F_LOG;
    int ret = -1;
    struct v4l2_control ctrl;

    ctrl.id = V4L2_CID_FLASH_LED_MODE;
    ctrl.value = mode;
    ret = ioctl(mCameraFd, VIDIOC_S_CTRL, &ctrl);
    if (ret < 0)
        LOGV("setFlashMode failed, %s", strerror(errno));
    else
        LOGV("setFlashMode ok");

    return ret;
}

// af init
int V4L2CameraDevice::setAutoFocusInit()
{
    F_LOG;
    int ret = -1;
    struct v4l2_control ctrl;

    ctrl.id = V4L2_CID_AUTO_FOCUS_INIT;
    ctrl.value = 0;
    ret = ioctl(mCameraFd, VIDIOC_S_CTRL, &ctrl);
    if (ret < 0)
        LOGE("setAutoFocusInit failed, %s", strerror(errno));
    else
        LOGV("setAutoFocusInit ok");

    return ret;
}

// af release
int V4L2CameraDevice::setAutoFocusRelease()
{
    F_LOG;
    int ret = -1;
    struct v4l2_control ctrl;

    ctrl.id = V4L2_CID_AUTO_FOCUS_RELEASE;
    ret = ioctl(mCameraFd, VIDIOC_S_CTRL, &ctrl);
    if (ret < 0)
        LOGE("setAutoFocusRelease failed, %s", strerror(errno));
    else
        LOGV("setAutoFocusRelease ok");

    return ret;
}

// af range
int V4L2CameraDevice::setAutoFocusRange(int af_range)
{
    F_LOG;
    int ret = -1;
    struct v4l2_control ctrl;

    ctrl.id = V4L2_CID_FOCUS_AUTO;
    ctrl.value = 1;
    ret = ioctl(mCameraFd, VIDIOC_S_CTRL, &ctrl);
    if (ret < 0)
        LOGV("setAutoFocusRange id V4L2_CID_FOCUS_AUTO failed, %s", strerror(errno));
    else
        LOGV("setAutoFocusRange id V4L2_CID_FOCUS_AUTO ok");

    ctrl.id = V4L2_CID_AUTO_FOCUS_RANGE;
    ctrl.value = af_range;
    ret = ioctl(mCameraFd, VIDIOC_S_CTRL, &ctrl);
    if (ret < 0)
        LOGV("setAutoFocusRange id V4L2_CID_AUTO_FOCUS_RANGE failed, %s", strerror(errno));
    else
        LOGV("setAutoFocusRange id V4L2_CID_AUTO_FOCUS_RANGE ok");

    return ret;
}

// af wind
int V4L2CameraDevice::setAutoFocusWind(int num, void *wind)
{
    F_LOG;
    int ret = -1;
    struct v4l2_control ctrl;

    ctrl.id = V4L2_CID_AUTO_FOCUS_WIN_NUM;
    ctrl.value = num;
    ctrl.user_pt = (unsigned int)wind;
    ret = ioctl(mCameraFd, VIDIOC_S_CTRL, &ctrl);
    if (ret < 0)
        LOGE("setAutoFocusCtrl failed, %s", strerror(errno));
    else
        LOGV("setAutoFocusCtrl ok");

    return ret;
}

// af start
int V4L2CameraDevice::setAutoFocusStart()
{
    F_LOG;
    int ret = -1;
    struct v4l2_control ctrl;

    ctrl.id = V4L2_CID_AUTO_FOCUS_START;
    ret = ioctl(mCameraFd, VIDIOC_S_CTRL, &ctrl);
    if (ret < 0)
        LOGE("setAutoFocusStart failed, %s", strerror(errno));
    else
        LOGV("setAutoFocusStart ok");

    return ret;
}

// af stop
int V4L2CameraDevice::setAutoFocusStop()
{
    F_LOG;
    int ret = -1;
    struct v4l2_control ctrl;

    ctrl.id = V4L2_CID_AUTO_FOCUS_STOP;
    ret = ioctl(mCameraFd, VIDIOC_S_CTRL, &ctrl);
    if (ret < 0)
        LOGE("setAutoFocusStart failed, %s", strerror(errno));
    else
        LOGV("setAutoFocusStart ok");

    return ret;
}

// get af statue
int V4L2CameraDevice::getAutoFocusStatus()
{
    //F_LOG;
    int ret = -1;
    struct v4l2_control ctrl;

    if (mCameraFd == (int)NULL)
    {
        return 0xFF000000;
    }

    ctrl.id = V4L2_CID_AUTO_FOCUS_STATUS;
    ret = ioctl(mCameraFd, VIDIOC_G_CTRL, &ctrl);
    if (ret >= 0)
    {
        //LOGV("getAutoFocusCtrl ok");
    }

    return ret;
}

int V4L2CameraDevice::getSnrValue()
{
    //F_LOG;
    int ret = -1;
    struct v4l2_control ctrl;
    struct v4l2_queryctrl qc_ctrl;

    if (mCameraFd == (int)NULL)
    {
        return 0xFF000000;
    }

    ctrl.id = V4L2_CID_GAIN;
    qc_ctrl.id = V4L2_CID_GAIN;

    if (-1 == ioctl (mCameraFd, VIDIOC_QUERYCTRL, &qc_ctrl))
    {
       return 0;
    }

    ret = ioctl(mCameraFd, VIDIOC_G_CTRL, &ctrl);
    return ret;
}


int V4L2CameraDevice::set3ALock(int lock)
{
    F_LOG;
    int ret = -1;
    struct v4l2_control ctrl;

    ctrl.id = V4L2_CID_3A_LOCK;
    ctrl.value = lock;
    ret = ioctl(mCameraFd, VIDIOC_S_CTRL, &ctrl);
    if (ret >= 0)
        LOGV("set3ALock ok");

    return ret;
}

int V4L2CameraDevice::v4l2setCaptureParams()
{
    F_LOG;
    int ret = -1;

    struct v4l2_streamparm params;

    CLEAR(params);
    params.parm.capture.timeperframe.numerator = 1;
    params.parm.capture.timeperframe.denominator = mFrameRate;
    if (mHalCameraInfo.is_uvc)
    {
        params.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    }else
    {
        #if USE_CSI_VIN_DRIVER //jiangwei
        params.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        #else
        params.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        #endif
    }

    if (mTakePictureState == TAKE_PICTURE_NORMAL)
    {
        params.parm.capture.capturemode = V4L2_MODE_IMAGE;
    }
    else
    {
        params.parm.capture.capturemode = V4L2_MODE_VIDEO;
    }

    LOGV("VIDIOC_S_PARM mFrameRate: %d, capture mode: %d",
        mFrameRate, params.parm.capture.capturemode);

    ret = ioctl(mCameraFd, VIDIOC_S_PARM, &params);
    if (ret < 0)
        LOGE("v4l2setCaptureParams failed, %s", strerror(errno));
    else
        LOGV("v4l2setCaptureParams ok");

    return ret;
}

int V4L2CameraDevice::enumSize(char * pSize, int len)
{
    struct v4l2_frmsizeenum size_enum;
    if (mHalCameraInfo.is_uvc)
    {
        size_enum.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    }else
    {
        #if USE_CSI_VIN_DRIVER //jiangwei
        size_enum.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        #else
        size_enum.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        #endif
    }
    size_enum.pixel_format = mCaptureFormat;

    if (pSize == NULL)
    {
        LOGE("error input params");
        return -1;
    }

    char str[16];
    memset(str, 0, 16);
    memset(pSize, 0, len);

    for(int i = 0; i < 20; i++)
    {
        size_enum.index = i;
        if (-1 == ioctl (mCameraFd, VIDIOC_ENUM_FRAMESIZES, &size_enum))
        {
            break;
        }
        // LOGV("format index = %d, size_enum: %dx%d",
        //    i, size_enum.discrete.width, size_enum.discrete.height);
        if(mCaptureFormat == V4L2_PIX_FMT_YUYV && (size_enum.discrete.width > 1920 || size_enum.discrete.height > 1080)) {
            LOGD(" (%d) x (%d) is too large for YUYV, discard it!", size_enum.discrete.width, size_enum.discrete.height);
            continue;
        }
        sprintf(str, "%dx%d", size_enum.discrete.width, size_enum.discrete.height);
        if (i != 0)
        {
            strcat(pSize, ",");
        }
        strcat(pSize, str);
    }

    return OK;
}

int V4L2CameraDevice::getFullSize(int * full_w, int * full_h)
{
    struct v4l2_frmsizeenum size_enum;

    if (mHalCameraInfo.is_uvc)
    {
        size_enum.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    }else
    {
        #if USE_CSI_VIN_DRIVER //jiangwei
        size_enum.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        #else
        size_enum.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        #endif
    }

    size_enum.pixel_format = mCaptureFormat;
    size_enum.index = 0;
    if (-1 == ioctl (mCameraFd, VIDIOC_ENUM_FRAMESIZES, &size_enum))
    {
        LOGE("getFullSize failed");
        //return -1;
    }

    *full_w = size_enum.discrete.width;
    *full_h = size_enum.discrete.height;

    LOGV("getFullSize: %dx%d", *full_w, *full_h);

    return OK;
}

int V4L2CameraDevice::getSuitableThumbScale(int full_w, int full_h)
{
    int scale = 1;
    if(mIsThumbUsedForVideo == true)
    {
        scale = 2;
    }
    if (full_w*full_h > 10*1024*1024)        //maybe 12m,13m,16m
        return 2;
    else if(full_w*full_h > 4.5*1024*1024)    //maybe 5m,8m
        return 2;
    else return scale;                        //others
#if 0
    if ((full_w == 4608)
        && (full_h == 3456))
    {
        return 4;    // 1000x750
    }
    if ((full_w == 3840)
        && (full_h == 2160))
    {
        return 4;    // 1000x750
    }
    if ((full_w == 4000)
        && (full_h == 3000))
    {
        return 4;    // 1000x750
    }
    else if ((full_w == 3264)
        && (full_h == 2448))
    {
        return 2;    // 1632x1224
    }
    else if ((full_w == 2592)
        && (full_h == 1936))
    {
        return 2;    // 1296x968
    }
    else if ((full_w == 1280)
        && (full_h == 960))
    {
        return 1 * scale;    // 1280x960
    }
    else if ((full_w == 1920)
        && (full_h == 1080))
    {
        return 2;    // 960x540
    }
    else if ((full_w == 1280)
        && (full_h == 720))
    {

        return 1 * scale;    // 1280x720
    }
    else if ((full_w == 640)
        && (full_h == 480))
    {
        return 1;    // 640x480
    }

    LOGW("getSuitableThumbScale unknown size: %dx%d", full_w, full_h);
    return 1;        // failed
#endif
}


}; /* namespace android */
