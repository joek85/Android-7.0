
/*
* Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : sbm.c
* Description :This is the stream buffer module. The SBM provides
*              methods for managing the stream data before decode.
* History :
*   Author  : xyliu <xyliu@allwinnertech.com>
*   Date    : 2016/04/13
*   Comment :
*
*
*/


#include <stdlib.h>
#include<string.h>
#include <pthread.h>
#include "sbm.h"
#include "CdcMessageQueue.h"

//#include "secureMemoryAdapter.h"

#include "log.h"

#define SBM_FRAME_FIFO_SIZE (2048)  //* store 2048 frames of bitstream data at maximum.
#define MAX_INVALID_STREAM_DATA_SIZE (1*1024*1024) //* 1 MB
#define MAX_NALU_NUM_IN_FRAME (1024)
#define min(x, y)   ((x) <= (y) ? (x) : (y));
#define ADD_ONE_FRAME   (0)

#define TMP_BUFFER_SIZE (1*1024*1024)

typedef struct StreamBufferManagerFrame
{
    SbmInterface    sbmInterface;

    pthread_mutex_t mutex;
    char*           pStreamBuffer;    //* start buffer address
    char*           pStreamBufferEnd;
    int             nStreamBufferSize; //* buffer total size
    char*           pWriteAddr;
    int             nValidDataSize;
    StreamFrameFifo frameFifo;

    CdcMessageQueue* mq;
    pthread_t        mThreadId;
    FramePicFifo     mFramePicFifo;
    sem_t            streamDataSem;
    sem_t            emptyFramePicSem;

    sem_t            resetSem;
    int              bStreamWithStartCode;
    DetectFramePicInfo mDetectInfo;
    SbmConfig mConfig;
    int       nEosFlag;

    int nDebugNum;

    char* pTmpBuffer;     //* for secure video
    int   nTmpBufferSize;

    char* pDetectBufStart;
    char* pDetectBufEnd;

}SbmFrameAvc;

static int lock(SbmFrameAvc *pSbm);
static void unlock(SbmFrameAvc *pSbm);

static void* ProcessThread(void* pThreadData);


enum SBM_THREAD_CMD
{
    SBM_THREAD_CMD_START = 0,
    SBM_THREAD_CMD_READ  = 1,
    SBM_THREAD_CMD_QUIT  = 2,
    SBM_THREAD_CMD_RESET = 3
};


#define IsFrameNalu(eNaluType) (eNaluType <= 5 || eNaluType == 20 || eNaluType == 14)

static int lock(SbmFrameAvc *pSbm)
{
    if(pthread_mutex_lock(&pSbm->mutex) != 0)
        return -1;
    return 0;
}

static void unlock(SbmFrameAvc *pSbm)
{
    pthread_mutex_unlock(&pSbm->mutex);
    return;
}


/*
**********************************************************************
*                             SbmCreate
*
*Description: Create Stream Buffer Manager module.
*
*Arguments  : nBufferSize     the size of pStreamBuffer, to store stream info.
*
*Return     : result
*               = NULL;     failed;
*              != NULL;     Sbm handler.
*
*Summary    : nBufferSize is between 4MB and 12MB.
*
**********************************************************************
*/
static int SbmFrameAvcInit(SbmInterface* pSelf, SbmConfig* pSbmConfig)
{
    SbmFrameAvc *pSbm = (SbmFrameAvc*)pSelf;
    char *pSbmBuf;
    int i;
    int ret;

    if(pSbmConfig == NULL)
    {
        loge(" pSbmConfig is null");
        return -1;
    }

    if(pSbmConfig->nSbmBufferTotalSize <= 0)
    {
        loge(" pSbmConfig->nBufferSize(%d) is invalid",pSbmConfig->nSbmBufferTotalSize);
        return -1;
    }

    memcpy(&pSbm->mConfig, pSbmConfig, sizeof(SbmConfig));

    pSbmBuf = (char*)CdcMemPalloc(pSbm->mConfig.memops, pSbm->mConfig.nSbmBufferTotalSize,
                                  pSbm->mConfig.veOpsS, pSbm->mConfig.pVeOpsSelf);//*
    if(pSbmBuf == NULL)
    {
        loge(" palloc for sbmBuf failed, size = %d MB",
                pSbm->mConfig.nSbmBufferTotalSize/1024/1024);
        goto ERROR;
    }

    pSbm->frameFifo.pFrames = (VideoStreamDataInfo *)malloc(SBM_FRAME_FIFO_SIZE
                                                 * sizeof(VideoStreamDataInfo));
    if(pSbm->frameFifo.pFrames == NULL)
    {
        loge("sbm->frameFifo.pFrames == NULL.");
        goto ERROR;
    }
    memset(pSbm->frameFifo.pFrames, 0,  SBM_FRAME_FIFO_SIZE * sizeof(VideoStreamDataInfo));
    for(i = 0; i < SBM_FRAME_FIFO_SIZE; i++)
    {
        pSbm->frameFifo.pFrames[i].nID = i;
    }

    ret = pthread_mutex_init(&pSbm->mutex, NULL);
    if(ret != 0)
    {
        loge("pthread_mutex_init failed.");
        goto ERROR;
    }
    pSbm->pStreamBuffer      = pSbmBuf;
    pSbm->pStreamBufferEnd   = pSbmBuf + pSbm->mConfig.nSbmBufferTotalSize - 1;
    pSbm->nStreamBufferSize  = pSbm->mConfig.nSbmBufferTotalSize;
    pSbm->pWriteAddr         = pSbmBuf;
    pSbm->nValidDataSize     = 0;

    pSbm->frameFifo.nMaxFrameNum     = SBM_FRAME_FIFO_SIZE;
    pSbm->frameFifo.nValidFrameNum   = 0;
    pSbm->frameFifo.nUnReadFrameNum  = 0;
    pSbm->frameFifo.nReadPos         = 0;
    pSbm->frameFifo.nWritePos        = 0;
    pSbm->frameFifo.nFlushPos        = 0;

    pSbm->mFramePicFifo.pFramePics = (FramePicInfo*)malloc(MAX_FRAME_PIC_NUM*sizeof(FramePicInfo));
    if(pSbm->mFramePicFifo.pFramePics == NULL)
    {
        loge("malloc for framePic failed");
        goto ERROR;
    }
    memset(pSbm->mFramePicFifo.pFramePics, 0, MAX_FRAME_PIC_NUM*sizeof(FramePicInfo));
    pSbm->mFramePicFifo.nMaxFramePicNum = MAX_FRAME_PIC_NUM;

    for(i = 0; i < MAX_FRAME_PIC_NUM; i++)
    {
        pSbm->mFramePicFifo.pFramePics[i].pNaluInfoList
            = (NaluInfo*)malloc(DEFAULT_NALU_NUM*sizeof(NaluInfo));
        if(pSbm->mFramePicFifo.pFramePics[i].pNaluInfoList == NULL)
        {
            loge("malloc for naluInfo failed");
            goto ERROR;
        }
        memset(pSbm->mFramePicFifo.pFramePics[i].pNaluInfoList, 0, \
                DEFAULT_NALU_NUM*sizeof(NaluInfo));
        pSbm->mFramePicFifo.pFramePics[i].nMaxNaluNum = DEFAULT_NALU_NUM;
    }

    if(pSbm->mConfig.bSecureVideoFlag == 1)
    {
        pSbm->pTmpBuffer = (char*)malloc(TMP_BUFFER_SIZE);
        if(pSbm->pTmpBuffer == NULL)
        {
            loge("malloc for pTmpBuffer failed");
            goto ERROR;
        }
        pSbm->nTmpBufferSize = TMP_BUFFER_SIZE;
    }

    if(pSbm->mConfig.bSecureVideoFlag == 1)
    {
        pSbm->pDetectBufStart = pSbm->pTmpBuffer;
        pSbm->pDetectBufEnd   = pSbm->pTmpBuffer + pSbm->nTmpBufferSize - 1;
    }
    else
    {
        pSbm->pDetectBufStart = pSbm->pStreamBuffer;
        pSbm->pDetectBufEnd   = pSbm->pStreamBufferEnd;
    }

    sem_init(&pSbm->streamDataSem, 0, 0);
    sem_init(&pSbm->emptyFramePicSem, 0, 0);
    sem_init(&pSbm->resetSem, 0, 0);

    pSbm->mq = CdcMessageQueueCreate(32, "sbm_Thread");

    int err = pthread_create(&pSbm->mThreadId, NULL, ProcessThread, pSbm);
    if(err || pSbm->mThreadId == 0)
    {
        loge("create sbm pthread failed");
        goto ERROR;
    }

    CdcMessage mMsg;
    memset(&mMsg, 0, sizeof(CdcMessage));
    mMsg.messageId = SBM_THREAD_CMD_READ;
    CdcMessageQueuePostMessage(pSbm->mq, &mMsg);

    pSbm->bStreamWithStartCode = -1;
    return 0;

ERROR:
    if(pSbmBuf)
        CdcMemPfree(pSbm->mConfig.memops,pSbmBuf,
                    pSbm->mConfig.veOpsS, pSbm->mConfig.pVeOpsSelf);

    if(pSbm->pTmpBuffer)
        free(pSbm->pTmpBuffer);

    if(pSbm)
    {

        sem_destroy(&pSbm->streamDataSem);
        sem_destroy(&pSbm->emptyFramePicSem);
        sem_destroy(&pSbm->resetSem);

        if(pSbm->frameFifo.pFrames)
            free(pSbm->frameFifo.pFrames);

        if(pSbm->mFramePicFifo.pFramePics)
        {
            for(i=0; i < MAX_FRAME_PIC_NUM; i++)
            {
                if(pSbm->mFramePicFifo.pFramePics[i].pNaluInfoList)
                    free(pSbm->mFramePicFifo.pFramePics[i].pNaluInfoList);
            }
            free(pSbm->mFramePicFifo.pFramePics);
        }
        free(pSbm);
    }

    return -1;

}


/*
**********************************************************************
*                             SbmDestroy
*
*Description: Destroy Stream Buffer Manager module, free resource.
*
*Arguments  : pSbm     Created by SbmCreate function.
*
*Return     : NULL
*
*Summary    :
*
**********************************************************************
*/
static void SbmFrameAvcDestroy(SbmInterface* pSelf)
{
    SbmFrameAvc* pSbm = (SbmFrameAvc*)pSelf;
    logv(" sbm destroy");
    if(pSbm != NULL)
    {
            //* send stop cmd to thread here
        CdcMessage msg;
        memset(&msg, 0, sizeof(CdcMessage));
        msg.messageId = SBM_THREAD_CMD_QUIT;
        CdcMessageQueuePostMessage(pSbm->mq,&msg);
        logv("*** post quit message");

        int error;
        pthread_join(pSbm->mThreadId, (void**)&error);
        logv("*** pthread_join finish");

        pthread_mutex_destroy(&pSbm->mutex);

        sem_destroy(&pSbm->streamDataSem);
        sem_destroy(&pSbm->emptyFramePicSem);
        sem_destroy(&pSbm->resetSem);

        if(pSbm->pStreamBuffer != NULL)
        {
            CdcMemPfree(pSbm->mConfig.memops,pSbm->pStreamBuffer,
                        pSbm->mConfig.veOpsS, pSbm->mConfig.pVeOpsSelf);
            pSbm->pStreamBuffer = NULL;
        }

        if(pSbm->frameFifo.pFrames != NULL)
        {
            free(pSbm->frameFifo.pFrames);
            pSbm->frameFifo.pFrames = NULL;
        }

        if(pSbm->mq)
            CdcMessageQueueDestroy(pSbm->mq);

        if(pSbm->mFramePicFifo.pFramePics)
        {
            int i = 0;
            for(i=0; i < MAX_FRAME_PIC_NUM; i++)
            {
                if(pSbm->mFramePicFifo.pFramePics[i].pNaluInfoList)
                    free(pSbm->mFramePicFifo.pFramePics[i].pNaluInfoList);
            }
            free(pSbm->mFramePicFifo.pFramePics);
        }

        if(pSbm->pTmpBuffer)
            free(pSbm->pTmpBuffer);

        free(pSbm);
    }
    logv(" sbm destroy finish");

    return;
}

/*
**********************************************************************
*                             SbmReset
*
*Description: Reset Stream Buffer Manager module.
*
*Arguments  : pSbm     Created by SbmCreate function.
*
*Return     : NULL
*
*Summary    : If succeed, Stream Buffer Manager module will be resumed to initial state,
*             stream data will be discarded.
*
**********************************************************************
*/
static void SbmFrameAvcReset(SbmInterface* pSelf)
{
    SbmFrameAvc* pSbm = (SbmFrameAvc*)pSelf;
    if(pSbm == NULL)
    {
        loge("pSbm == NULL.");
        return;
    }

    CdcMessage mMsg;
    memset(&mMsg, 0, sizeof(CdcMessage));
    mMsg.messageId = SBM_THREAD_CMD_RESET;
    mMsg.params[0] = (uintptr_t)(&pSbm->resetSem);
    CdcMessageQueuePostMessage(pSbm->mq, &mMsg);

    logd("** wait for reset sem");
    SemTimedWait(&pSbm->resetSem, -1);
    logd("** wait for reset sem ok");

    memset(&mMsg, 0, sizeof(CdcMessage));
    mMsg.messageId = SBM_THREAD_CMD_READ;
    CdcMessageQueuePostMessage(pSbm->mq, &mMsg);
    logd("SbmFrameReset finish");
    return;
}

/*
**********************************************************************
*                             SbmBufferAddress
*
*Description: Get the base address of SBM buffer.
*
*Arguments  : pSbm     Created by SbmCreate function.
*
*Return     : The base address of SBM buffer.
*
*Summary    :
*
**********************************************************************
*/
static void *SbmFrameAvcGetBufferAddress(SbmInterface* pSelf)
{
    SbmFrameAvc *pSbm = (SbmFrameAvc*)pSelf;
    if(pSbm == NULL)
    {
        loge("pSbm == NULL.");
        return NULL;
    }

    return pSbm->pStreamBuffer;
}

/*
**********************************************************************
*                             SbmBufferSize
*
*Description: Get the sbm buffer size.
*
*Arguments  : pSbm     Created by SbmCreate function.
*
*Return     : The size of SBM buffer, in Bytes.
*
*Summary    : The size is set when create SBM.
*
**********************************************************************
*/
static int SbmFrameAvcGetBufferSize(SbmInterface* pSelf)
{
    SbmFrameAvc* pSbm = (SbmFrameAvc*)pSelf;

    if(pSbm == NULL)
    {
        loge("pSbm == NULL.");
        return 0;
    }

    return pSbm->nStreamBufferSize;
}

/*
**********************************************************************
*                             SbmStreamFrameNum
*
*Description: Get the total frames of undecoded stream data.
*
*Arguments  : pSbm     Created by SbmCreate function.
*
*Return     : The frames of undecoded stream data.
*
*Summary    :
*
**********************************************************************
*/
static int SbmFrameAvcGetStreamFrameNum(SbmInterface* pSelf)
{
    SbmFrameAvc *pSbm = (SbmFrameAvc*)pSelf;

    if(pSbm == NULL)
    {
        loge("pSbm == NULL.");
        return 0;
    }

    return pSbm->frameFifo.nValidFrameNum + pSbm->mFramePicFifo.nValidFramePicNum;
}

/*
**********************************************************************
*                             SbmStreamDataSize
*
*Description: Get the total size of undecoded data.
*
*Arguments  : pSbm     Created by SbmCreate function.
*
*Return     : The total size of undecoded stream data, in bytes.
*
*Summary    :
*
**********************************************************************
*/
static int SbmFrameAvcGetStreamDataSize(SbmInterface* pSelf)
{
    SbmFrameAvc* pSbm = (SbmFrameAvc*)pSelf;

    if(pSbm == NULL)
    {
        loge("pSbm == NULL.");
        return 0;
    }

    return pSbm->nValidDataSize;
}

static char* SbmFrameAvcGetBufferWritePointer(SbmInterface* pSelf)
{
    SbmFrameAvc* pSbm = (SbmFrameAvc*)pSelf;

    if(pSbm == NULL)
    {
        loge("pSbm == NULL.");
        return 0;
    }

    return pSbm->pWriteAddr;
}

static void* SbmFrameAvcGetBufferDataInfo(SbmInterface* pSelf)
{
    SbmFrameAvc* pSbm = (SbmFrameAvc*)pSelf;
    FramePicInfo* pFramePic = NULL;

    if(pSbm == NULL )
    {
        loge("pSbm == NULL.");
        return NULL;
    }

    if(lock(pSbm) != 0)
    {
        return NULL;
    }

    if(pSbm->mFramePicFifo.nUnReadFramePicNum == 0)
    {
        logv("nUnReadFrameNum == 0.");
        unlock(pSbm);
        return NULL;
    }
    pFramePic = &pSbm->mFramePicFifo.pFramePics[pSbm->mFramePicFifo.nFPReadPos];
    if(pFramePic == NULL)
    {
        loge("request failed.");
        unlock(pSbm);
        return NULL;
    }

    unlock(pSbm);
    return pFramePic->pVideoInfo;
}
/*
**********************************************************************
*                             SbmRequestBuffer
*
*Description: Request buffer from sbm module.
*
*Arguments  : pSbm              Created by SbmCreate function;
*             nRequireSize      the required size, in bytes;
*             ppBuf             store the requested buffer address;
*             pBufSize          store the requested buffer size.
*
*Return     : result;
*               = 0;    succeeded;
*               = -1;   failed.
*
*Summary    : SBM buffer is cyclic, if the  buffer turns around, there will be 2 blocks.
*
**********************************************************************
*/
static int SbmFrameAvcRequestBuffer(SbmInterface* pSelf, int nRequireSize,
                                  char **ppBuf, int *pBufSize)
{
    SbmFrameAvc *pSbm = (SbmFrameAvc*)pSelf;

    if(pSbm == NULL || ppBuf == NULL || pBufSize == NULL)
    {
        loge("input error.");
        return -1;
    }

    if(lock(pSbm) != 0)
        return -1;

    if(pSbm->frameFifo.nValidFrameNum >= pSbm->frameFifo.nMaxFrameNum)
    {
        logd("nValidFrameNum >= nMaxFrameNum.");
        unlock(pSbm);
        return -1;
    }

    if(pSbm->nValidDataSize < pSbm->nStreamBufferSize)
    {
        int nFreeSize = pSbm->nStreamBufferSize - pSbm->nValidDataSize;
        if((nRequireSize + 64) > nFreeSize)
        {
            unlock(pSbm);
            return -1;
        }

        *ppBuf    = pSbm->pWriteAddr;
        *pBufSize = nRequireSize;

        unlock(pSbm);
        return 0;
    }
    else
    {
        loge("no free buffer.");
        unlock(pSbm);
        return -1;
    }
}

/*
**********************************************************************
*                             SbmAddStream
*
*Description: Add one frame stream to sbm module.
*
*Arguments  : pSbm              Created by SbmCreate function;
*             pDataInfo         the stream info need to be added.
*
*Return     : result;
*               = 0;    succeeded;
*               = -1;   failed.
*
*Summary    : pDataInfo should contain Complete frame, bIsFirstPart=bIsLastPart=1.
*
**********************************************************************
*/
static int SbmFrameAvcAddStream(SbmInterface* pSelf, VideoStreamDataInfo *pDataInfo)
{
    SbmFrameAvc *pSbm = (SbmFrameAvc*)pSelf;

    int nWritePos;
    char *pNewWriteAddr;

    if(pSbm == NULL || pDataInfo == NULL)
    {
        loge("input error.");
        return -1;
    }

    if(lock(pSbm) != 0)
        return -1;

    if(pDataInfo->pData == 0)
    {
        loge("data buffer is NULL.\n");
        unlock(pSbm);
        return -1;
    }
    if(pSbm->frameFifo.nValidFrameNum >= pSbm->frameFifo.nMaxFrameNum)
    {
        loge("nValidFrameNum > nMaxFrameNum.");
        unlock(pSbm);
        return -1;
    }

    if(pDataInfo->nLength + pSbm->nValidDataSize > pSbm->nStreamBufferSize)
    {
        loge("no free buffer.");
        unlock(pSbm);
        return -1;
    }
    if(pDataInfo->bValid == 0)
    {
        pDataInfo->bValid = 1;
    }

    nWritePos = pSbm->frameFifo.nWritePos;
    memcpy(&pSbm->frameFifo.pFrames[nWritePos], pDataInfo, sizeof(VideoStreamDataInfo));
    nWritePos++;
    if(nWritePos >= pSbm->frameFifo.nMaxFrameNum)
    {
        nWritePos = 0;
    }

    pSbm->frameFifo.nWritePos = nWritePos;
    pSbm->frameFifo.nValidFrameNum++;
    pSbm->frameFifo.nUnReadFrameNum++;
    pSbm->nValidDataSize += pDataInfo->nLength;

    pNewWriteAddr = pSbm->pWriteAddr + pDataInfo->nLength;
    if(pNewWriteAddr > pSbm->pStreamBufferEnd)
    {
        pNewWriteAddr -= pSbm->nStreamBufferSize;
    }

    pSbm->pWriteAddr = pNewWriteAddr;

    int nSemCnt = 0;
    if(sem_getvalue(&pSbm->streamDataSem, &nSemCnt) == 0)
    {
        if(nSemCnt == 0)
            sem_post(&pSbm->streamDataSem);
    }

    unlock(pSbm);
    return 0;
}

//* in fact, this is the function of requestFramePic
static VideoStreamDataInfo* SbmFrameAvcRequestFramePic(SbmInterface* pSelf) //* requestFramePic
{
    SbmFrameAvc* pSbm = (SbmFrameAvc*)pSelf;
    FramePicInfo* pFramePic = NULL;

    if(pSbm == NULL )
    {
        loge("the_222,pSbm == NULL.");
        return NULL;
    }

    if(lock(pSbm) != 0)
        return NULL;

    if(pSbm->mFramePicFifo.nUnReadFramePicNum == 0)
    {
        logv("nUnReadFramePicNum == 0.");
        unlock(pSbm);
        return NULL;
    }

    pFramePic = &pSbm->mFramePicFifo.pFramePics[pSbm->mFramePicFifo.nFPReadPos];
    if(pFramePic == NULL)
    {
        loge("request framePic failed");
        unlock(pSbm);
        return NULL;
    }

    pSbm->mFramePicFifo.nFPReadPos++;
    pSbm->mFramePicFifo.nUnReadFramePicNum--;
    if(pSbm->mFramePicFifo.nFPReadPos >= pSbm->mFramePicFifo.nMaxFramePicNum)
    {
        pSbm->mFramePicFifo.nFPReadPos = 0;
    }
    logv("sbm request stream, pos = %d, pFrame = %p, pts = %lld",
          pSbm->mFramePicFifo.nFPReadPos,pFramePic, pFramePic->nPts);
    unlock(pSbm);
    return (VideoStreamDataInfo*)pFramePic;
}

//* in fact, this is the function of returnFramePic
static int SbmFrameAvcReturnFramePic(SbmInterface* pSelf,
                                                VideoStreamDataInfo *pDataInfo)//* returnFramePic
{
    SbmFrameAvc* pSbm = (SbmFrameAvc*)pSelf;
    FramePicInfo* pFramePic = (FramePicInfo*)pDataInfo;
    int nReadPos;

    if(pSbm == NULL || pFramePic == NULL)
    {
        loge("input error.");
        return -1;
    }

    if(lock(pSbm) != 0)
        return -1;

    if(pSbm->mFramePicFifo.nValidFramePicNum == 0)
    {
        loge("nValidFrameNum == 0.");
        unlock(pSbm);
        return -1;
    }

    nReadPos = pSbm->mFramePicFifo.nFPReadPos;
    nReadPos--;
    if(nReadPos < 0)
    {
        nReadPos = pSbm->mFramePicFifo.nMaxFramePicNum - 1;
    }

    if(pFramePic != &pSbm->mFramePicFifo.pFramePics[nReadPos])
    {
        loge("wrong frame pic sequence.");
        abort();
    }

    pSbm->mFramePicFifo.nFPReadPos = nReadPos;
    pSbm->mFramePicFifo.nUnReadFramePicNum++;
    unlock(pSbm);
    return 0;
}

//* in fact, this is the function of flushFramePic
static int SbmFrameAvcFlushFramePic(SbmInterface* pSelf,
                                               VideoStreamDataInfo *pDataInfo) //* flushFramePic
{
    SbmFrameAvc* pSbm = (SbmFrameAvc*)pSelf;

    FramePicInfo* pFramePic = (FramePicInfo*)pDataInfo;
    int nFlushPos;

    if(pSbm == NULL || pFramePic == NULL)
    {
        loge("input error");
        return -1;
    }

    if(lock(pSbm) != 0)
        return -1;

    if(pSbm->mFramePicFifo.nValidFramePicNum == 0)
    {
        logw("nValidFrameNum == 0.");
        unlock(pSbm);
        return -1;
    }
    nFlushPos = pSbm->mFramePicFifo.nFPFlushPos;
    logv("the_333,sbm flush stream , pos = %d, pFrame = %p, %p",nFlushPos,
          pFramePic, &pSbm->mFramePicFifo.pFramePics[nFlushPos]);
    if(pFramePic != &pSbm->mFramePicFifo.pFramePics[nFlushPos])
    {
        loge("not current nFlushPos.");
        abort();
    }

    nFlushPos++;
    if(nFlushPos >= pSbm->mFramePicFifo.nMaxFramePicNum)
    {
        nFlushPos = 0;
    }

    pSbm->mFramePicFifo.nValidFramePicNum--;
    pSbm->mFramePicFifo.nFPFlushPos = nFlushPos;
    pSbm->nValidDataSize -= pFramePic->nlength;

    int nSemCnt = 0;
    if(sem_getvalue(&pSbm->emptyFramePicSem, &nSemCnt) == 0)
    {
        if(nSemCnt == 0)
            sem_post(&pSbm->emptyFramePicSem);
    }
    unlock(pSbm);
    return 0;
}

static int SbmFrameAvcSetEos(SbmInterface* pSelf, int nEosFlag)
{
    SbmFrameAvc* pSbm = (SbmFrameAvc*)pSelf;
    if(pSbm == NULL)
    {
        logw("set eos failed");
        return -1;
    }
    pSbm->nEosFlag = nEosFlag;

    return 0;
}

/*
**********************************************************************
*                             SbmRequestStream
*
*Description: Request one frame stream data from sbm module to decoder.
*
*Arguments  : pSbm      Created by SbmCreate function;
*
*Return     : The stream infomation.
*
*Summary    : The stream data obeys FIFO rule.
*
**********************************************************************
*/
static VideoStreamDataInfo *requestStream(SbmFrameAvc *pSbm)
{
    VideoStreamDataInfo *pDataInfo;

    if(pSbm == NULL )
    {
        loge("pSbm == NULL.");
        return NULL;
    }

    if(lock(pSbm) != 0)
    {
        return NULL;
    }

    if(pSbm->frameFifo.nUnReadFrameNum == 0)
    {
        logv("nUnReadFrameNum == 0.");
        unlock(pSbm);
        return NULL;
    }

    pDataInfo = &pSbm->frameFifo.pFrames[pSbm->frameFifo.nReadPos];

    if(pDataInfo == NULL)
    {
        loge("request failed.");
        unlock(pSbm);
        return NULL;
    }

    pSbm->frameFifo.nReadPos++;
    pSbm->frameFifo.nUnReadFrameNum--;
    if(pSbm->frameFifo.nReadPos >= pSbm->frameFifo.nMaxFrameNum)
    {
        pSbm->frameFifo.nReadPos = 0;
    }
    unlock(pSbm);
/*
    logv("*** reqeust stream, pDataInfo = %p, pos = %d",pDataInfo, pSbm->frameFifo.nReadPos - 1);
    logv("*** reqeust stream, data: %x %x %x %x %x %x %x %x ",
         pDataInfo->pData[0], pDataInfo->pData[1],pDataInfo->pData[2],pDataInfo->pData[3],
         pDataInfo->pData[4],pDataInfo->pData[5],pDataInfo->pData[6],pDataInfo->pData[7]);
*/
    return pDataInfo;
}

/*
**********************************************************************
*                             SbmReturnStream
*
*Description: Return one undecoded frame to sbm module.
*
*Arguments  : pSbm          Created by SbmCreate function;
*             pDataInfo     the stream info need to be returned.
*
*Return     : result;
*               = 0;    succeeded;
*               = -1;   failed.
*
*Summary    : After returned, the stream data's sequence is the same as before.
*
**********************************************************************
*/
static int returnStream(SbmFrameAvc* pSbm , VideoStreamDataInfo *pDataInfo)
{
    int nReadPos;

    if(pSbm == NULL || pDataInfo == NULL)
    {
        loge("input error.");
        return -1;
    }

    if(lock(pSbm) != 0)
    {
        return -1;
    }

    if(pSbm->frameFifo.nValidFrameNum == 0)
    {
        loge("nValidFrameNum == 0.");
        unlock(pSbm);
        return -1;
    }
    nReadPos = pSbm->frameFifo.nReadPos;
    nReadPos--;
    if(nReadPos < 0)
    {
        nReadPos = pSbm->frameFifo.nMaxFrameNum - 1;
    }
    pSbm->frameFifo.nUnReadFrameNum++;
    if(pDataInfo != &pSbm->frameFifo.pFrames[nReadPos])
    {
        loge("wrong frame sequence.");
        abort();
    }

    pSbm->frameFifo.pFrames[nReadPos] = *pDataInfo;
    pSbm->frameFifo.nReadPos  = nReadPos;

    unlock(pSbm);
    return 0;
}

/*
**********************************************************************
*                             SbmFlushStream
*
*Description: Flush one frame which is requested from SBM.
*
*Arguments  : pSbm          Created by SbmCreate function;
*             pDataInfo     the stream info need to be flushed.
*
*Return     : result;
*               = 0;    succeeded;
*               = -1;   failed.
*
*Summary    : After flushed, the buffer can be used to store new stream.
*
**********************************************************************
*/
static int flushStream(SbmFrameAvc *pSbm, VideoStreamDataInfo *pDataInfo, int bFlush)
{
    int nFlushPos;

    if(pSbm == NULL)
    {
        loge("pSbm == NULL.");
        return -1;
    }

    if(lock(pSbm) != 0)
    {
        return -1;
    }

    if(pSbm->frameFifo.nValidFrameNum == 0)
    {
        loge("no valid frame., flush pos = %d, pDataInfo = %p",
             pSbm->frameFifo.nFlushPos, pDataInfo);
        unlock(pSbm);
        return -1;
    }

    nFlushPos = pSbm->frameFifo.nFlushPos;

    logv("flush stream, pDataInfo = %p, pos = %d, %p",pDataInfo, nFlushPos, \
            &pSbm->frameFifo.pFrames[nFlushPos]);
    if(pDataInfo != &pSbm->frameFifo.pFrames[nFlushPos])
    {
        loge("not current nFlushPos.");
        unlock(pSbm);
        abort();
        return -1;
    }

    nFlushPos++;
    if(nFlushPos >= pSbm->frameFifo.nMaxFrameNum)
    {
        nFlushPos = 0;
    }

    pSbm->frameFifo.nValidFrameNum--;
    if(bFlush)
       pSbm->nValidDataSize     -= pDataInfo->nLength;
    pSbm->frameFifo.nFlushPos = nFlushPos;   //*
    unlock(pSbm);
    return 0;
}

static FramePicInfo* requestEmptyFrame(SbmFrameAvc* pSbm)
{
    int nWritePos = -1;
    FramePicInfo* pFramePic = NULL;

    if(pSbm == NULL)
    {
        loge("pSbm == NULL.");
        return NULL;
    }

    if(lock(pSbm) != 0)
        return NULL;

    if(pSbm->mFramePicFifo.nValidFramePicNum >= pSbm->mFramePicFifo.nMaxFramePicNum)
    {
        logv("no emptye framePic");
        unlock(pSbm);
        return NULL;
    }

    nWritePos = pSbm->mFramePicFifo.nFPWritePos;
    pFramePic = &pSbm->mFramePicFifo.pFramePics[nWritePos];
    pFramePic->bVideoInfoFlag = 0;
    unlock(pSbm);
    logv("request empty frame pic, pos = %d, pFramePic = %p",nWritePos, pFramePic);
    return pFramePic;
}

static int addFramePic(SbmFrameAvc* pSbm, FramePicInfo* pFramePic) //* addFramePic
{
    int nWritePos = -1;

    if(pSbm == NULL || pFramePic == NULL)
    {
        loge("the_444,error input");
        return -1;
    }

    if(lock(pSbm) != 0)
        return -1;

    if(pSbm->mFramePicFifo.nValidFramePicNum >= pSbm->mFramePicFifo.nMaxFramePicNum)
    {
        loge("the_444,nValidFrameNum >= nMaxFrameNum.");
        unlock(pSbm);
        return -1;
    }

    nWritePos = pSbm->mFramePicFifo.nFPWritePos;
    if(pFramePic != &pSbm->mFramePicFifo.pFramePics[nWritePos])
    {
        loge("the_444, frame pic is not match: %p, %p, %d",
              pFramePic, &pSbm->mFramePicFifo.pFramePics[nWritePos], nWritePos);
        abort();
    }

    nWritePos++;
    if(nWritePos >= pSbm->mFramePicFifo.nMaxFramePicNum)
    {
        nWritePos = 0;
    }

    pSbm->mFramePicFifo.nFPWritePos = nWritePos;
    pSbm->mFramePicFifo.nValidFramePicNum++;
    pSbm->mFramePicFifo.nUnReadFramePicNum++;
    unlock(pSbm);
    return 0;
}

static inline u8 getbits8(char* buffer, u32 start, u8 len)
{
    u32 i = 0;
    u8  n = 0;
    u8  w = 0;
    u8  k = 0;
    u8  ret = 0;

    n = start % 8;
    i = start / 8;
    w = 8 - n;
    k = (len > w ? len - w : 0);

    ret = (buffer[i] << n);
    if(8 > len)
        ret  >>= (8-len);
    if(k)
        ret |= (buffer[i+1] >> (8-k));

    return  ret;
}

static inline u32 readGolomb(char* buffer, u32* init)
{
    u32 y, w = 0, w2 = 0, k, len = 0, m = *init;

    while(getbits8(buffer, m++, 1) == 0)
        len++;

    y = len + m;
    while(m < y)
    {
        k = min(y - m, 8);
        w |= getbits8(buffer, m, k);
        m += k;
        if(y - m > 8)
            w <<= 8;
    }

    w2 = 1;
    for(m = 0; m < len; m++)
        w2 <<= 1;
    w2 = (w2 - 1) + w;

    //fprintf(stderr, "READ_GOLOMB(%u), V=2^%u + %u-1 = %u\n", *init, len, w, w2);
    *init = y;
    return w2;
}

static inline char readByteIdx(char *p, char *pStart, char *pEnd, s32 i)
{
    logv("p = %p, start = %p, end = %p, i = %d",p,pStart, pEnd, i);
    char c = 0x0;
    if((p+i) <= pEnd)
        c = p[i];
    else
    {
        s32 d = (s32)(pEnd - p) + 1;
        c = pStart[i - d];
    }
    return c;
}

static inline void ptrPlusOne(char **p, char *pStart, char *pEnd)
{
    if((*p) == pEnd)
        (*p) = pStart;
    else
        (*p) += 1;
}

static s32 checkTmpBufferIsEnough(SbmFrameAvc* pSbm, int nSize)
{
    if(pSbm->nTmpBufferSize < nSize)
    {
        pSbm->nTmpBufferSize += TMP_BUFFER_SIZE;

        logw("** tmp buffer is not enough, remalloc to : %d MB",pSbm->nTmpBufferSize/1024/1024);

        free(pSbm->pTmpBuffer);

        pSbm->pTmpBuffer = (char*)malloc(pSbm->nTmpBufferSize);
        if(pSbm->pTmpBuffer == NULL)
        {
            loge("*** malloc for tmp buffer failed");
            abort();
        }

        pSbm->pDetectBufStart = pSbm->pTmpBuffer;
        pSbm->pDetectBufEnd   = pSbm->pTmpBuffer + pSbm->nTmpBufferSize - 1;
    }
    return 0;
}

static s32 selectCheckBuffer(SbmFrameAvc* pSbm,VideoStreamDataInfo *pStream, char** ppBuf)
{
    if(pSbm->mConfig.bSecureVideoFlag == 1)
    {
        checkTmpBufferIsEnough(pSbm, pStream->nLength);

        int nRingBufSize = pSbm->pStreamBufferEnd - pStream->pData + 1;

        if(pStream->nLength <= nRingBufSize)
        {
            CdcMemRead(pSbm->mConfig.memops, pSbm->pTmpBuffer, pStream->pData, pStream->nLength);
        }
        else
        {
            logw("** buffer ring, %d, %d",pStream->nLength, nRingBufSize);
            CdcMemRead(pSbm->mConfig.memops, pSbm->pTmpBuffer, pStream->pData, nRingBufSize);
            CdcMemRead(pSbm->mConfig.memops, pSbm->pTmpBuffer + nRingBufSize,
                       pSbm->pStreamBuffer, pStream->nLength - nRingBufSize);
        }
        *ppBuf   = pSbm->pTmpBuffer;
    }
    else
    {
        *ppBuf   = pStream->pData;
    }
    return 0;
}

static s32 checkBitStreamTypeWithStartCode(SbmFrameAvc* pSbm,
                                           VideoStreamDataInfo *pStream)
{
    char *pBuf = NULL;
    char tmpBuf[4] = {0};
    const s32 nTsStreamType       = 0x000001;
    char* pStart = pSbm->pDetectBufStart;
    char* pEnd   = pSbm->pDetectBufEnd;
    s32 nHadCheckBytesLen = 0;
    s32 nCheck4BitsValue = -1;
    s32 nBufSize = pStream->nLength;
    //*1. process sbm-cycle-buffer case
    selectCheckBuffer(pSbm, pStream, &pBuf);

    while((nHadCheckBytesLen + 4) < nBufSize)
    {

        tmpBuf[0] = readByteIdx(pBuf, pStart, pEnd, nHadCheckBytesLen + 0);
        tmpBuf[1] = readByteIdx(pBuf, pStart, pEnd, nHadCheckBytesLen + 1);
        tmpBuf[2] = readByteIdx(pBuf, pStart, pEnd, nHadCheckBytesLen + 2);
        tmpBuf[3] = readByteIdx(pBuf, pStart, pEnd, nHadCheckBytesLen + 3);

        nCheck4BitsValue = (tmpBuf[0] << 24) | (tmpBuf[1] << 16) | (tmpBuf[2] << 8) | tmpBuf[3];
        if(nCheck4BitsValue == 0) //*compatible for the case: 00 00 00 00 00 00 00 01
        {
            nHadCheckBytesLen++;
            continue;
        }

        if(nCheck4BitsValue == nTsStreamType)
        {
            pSbm->bStreamWithStartCode = 1;
            return 0;
        }
        else if((nCheck4BitsValue >> 8) == nTsStreamType)
        {
            pSbm->bStreamWithStartCode = 1;
            return 0;
        }
        else
        {
            nHadCheckBytesLen += 4;
            continue;
        }
    }

    return -1;
}

static s32 checkBitStreamTypeWithoutStartCode(SbmFrameAvc* pSbm,
                                           VideoStreamDataInfo *pStream)
{
    char *pBuf = NULL;
    char tmpBuf[4] = {0};
    s32 nDataSize   = -1;
    s32 nRemainSize = -1;
    s32 nRet = -1;
    char* pStart = pSbm->pDetectBufStart;
    char* pEnd   = pSbm->pDetectBufEnd;

    s32 nHadProcessLen = 0;

    selectCheckBuffer(pSbm, pStream, &pBuf);

    while(nHadProcessLen < pStream->nLength)
    {
        nRemainSize = pStream->nLength-nHadProcessLen;
        tmpBuf[0] = readByteIdx(pBuf, pStart, pEnd, 0);
        tmpBuf[1] = readByteIdx(pBuf, pStart, pEnd, 1);
        tmpBuf[2] = readByteIdx(pBuf, pStart, pEnd, 2);
        tmpBuf[3] = readByteIdx(pBuf, pStart, pEnd, 3);
        nDataSize = (tmpBuf[0] << 24) | (tmpBuf[1] << 16) | (tmpBuf[2] << 8) | tmpBuf[3];
        if(nDataSize > (nRemainSize - 4)|| nDataSize < 0)
        {
            nRet = -1;
            break;
        }
        logv("*** nDataSize = %d, nRemainSize = %d, proceLen = %d, totalLen = %d",
            nDataSize, nRemainSize,
            nHadProcessLen,pStream->nLength);

        if(nDataSize == (nRemainSize - 4) && nDataSize != 0)
        {
            nRet = 0;
            break;
        }

        nHadProcessLen += nDataSize + 4;
        pBuf += nDataSize + 4;
    }

    return nRet;
}

static s32 checkBitStreamType(SbmFrameAvc* pSbm)
{
    const s32 nUpLimitCount       = 50;
    s32 nReqeustCounter  = 0;
    s32 nRet = VDECODE_RESULT_NO_BITSTREAM;
    s32 bStartCode_with = 0;
    s32 bStartCode_without = 0;

    while(nReqeustCounter < nUpLimitCount)
    {
        VideoStreamDataInfo *pStream = NULL;
        nReqeustCounter++;
        pStream = requestStream(pSbm);
        if(pStream == NULL)
        {
            nRet = VDECODE_RESULT_NO_BITSTREAM;
            break;
        }
        if(pStream->nLength == 0 || pStream->pData == NULL)
        {
            flushStream(pSbm, pStream, 1);
            pStream = NULL;
            continue;
        }

        if(checkBitStreamTypeWithStartCode(pSbm, pStream) == 0)
        {
            bStartCode_with = 1;
        }
        else
        {
            bStartCode_with = 0;
        }

        if(checkBitStreamTypeWithoutStartCode(pSbm, pStream) == 0)
        {
            bStartCode_without = 1;
        }
        else
        {
            bStartCode_without = 0;
        }

        if(bStartCode_with == 1 && bStartCode_without == 1)
        {
            pSbm->bStreamWithStartCode = 0;
        }
        else if(bStartCode_with == 1 && bStartCode_without == 0)
        {
            pSbm->bStreamWithStartCode = 1;
        }
        else if(bStartCode_with == 0 && bStartCode_without == 1)
        {
            pSbm->bStreamWithStartCode = 0;
        }
        else
        {
           pSbm->bStreamWithStartCode = -1;
        }

        logd("result: bStreamWithStartCode[%d], with[%d], whitout[%d]",
              pSbm->bStreamWithStartCode,
              bStartCode_with, bStartCode_without);

        //*continue reqeust stream from sbm when if judge the stream type
        if(pSbm->bStreamWithStartCode == -1)
        {
            flushStream(pSbm, pStream, 1);
            continue;
        }
        else
        {
            //* judge stream type successfully, return.
            returnStream(pSbm, pStream);
            nRet = 0;
            break;
        }
    }

    return nRet;
}

static void expandNaluList(FramePicInfo* pFramePic)
{
    logd("nalu num for one frame is not enought, expand it: %d, %d",
          pFramePic->nMaxNaluNum, pFramePic->nMaxNaluNum + DEFAULT_NALU_NUM);

    pFramePic->nMaxNaluNum += DEFAULT_NALU_NUM;
    pFramePic->pNaluInfoList = realloc(pFramePic->pNaluInfoList,
                                       pFramePic->nMaxNaluNum*sizeof(NaluInfo));

}

static void chooseFramePts(DetectFramePicInfo* pDetectInfo)
{
    int i;
    pDetectInfo->pCurFramePic->nPts = -1;
    for(i=0; i < MAX_FRAME_PTS_LIST_NUM; i++)
    {
        logv("*** choose pts: %lld, i = %d",pDetectInfo->nCurFramePtsList[i], i);
        if(pDetectInfo->nCurFramePtsList[i] != -1)
        {
            pDetectInfo->pCurFramePic->nPts = pDetectInfo->nCurFramePtsList[i];
            break;
        }
    }
}

static void initFramePicInfo(DetectFramePicInfo* pDetectInfo)
{
    FramePicInfo* pFramePic = pDetectInfo->pCurFramePic;
    pFramePic->bValidFlag = 1;
    pFramePic->nlength = 0;
    pFramePic->pDataStartAddr = NULL;
    pFramePic->nPts = -1;
    pFramePic->nPcr = -1;
    pFramePic->nCurNaluIdx = 0;

    int i;
    for(i = 0; i < MAX_FRAME_PTS_LIST_NUM; i++)
        pDetectInfo->nCurFramePtsList[i] = -1;

    if(pFramePic->nMaxNaluNum > DEFAULT_NALU_NUM)
    {
        pFramePic->nMaxNaluNum   = DEFAULT_NALU_NUM;
        pFramePic->pNaluInfoList = realloc(pFramePic->pNaluInfoList,
                                           pFramePic->nMaxNaluNum*sizeof(NaluInfo));
    }

    memset(pFramePic->pNaluInfoList, 0, pFramePic->nMaxNaluNum*sizeof(NaluInfo));

}

static int searchStartCode(SbmFrameAvc* pSbm, int* pAfterStartCodeIdx)
{
    char* pStart = pSbm->pDetectBufStart;
    char* pEnd   = pSbm->pDetectBufEnd;

    DetectFramePicInfo* pDetectInfo = &pSbm->mDetectInfo;

    char *pBuf = pDetectInfo->pCurStreamDataptr;
    s32 nSize = pDetectInfo->nCurStreamDataSize - 3;

    if(pDetectInfo->nCurStreamRebackFlag)
    {
        logv("bHasTwoDataTrunk pSbmBuf: %p, pSbmBufEnd: %p, curr: %p, diff: %d ",
                pStart, pEnd, pBuf, (u32)(pEnd - pBuf));
        while(nSize > 0)
        {
            char tmpBuf[3];
            tmpBuf[0] = readByteIdx(pBuf , pStart, pEnd, 0);
            tmpBuf[1] = readByteIdx(pBuf , pStart, pEnd, 1);
            tmpBuf[2] = readByteIdx(pBuf , pStart, pEnd, 2);
            if(tmpBuf[0] == 0 && tmpBuf[1] == 0 && tmpBuf[2] == 1)
            {
                (*pAfterStartCodeIdx) += 3; //so that buf[0] is the actual data, not start code
                return 0;
            }
            ptrPlusOne(&pBuf, pStart, pEnd);
            ++(*pAfterStartCodeIdx);
            --nSize;
        }
    }
    else
    {
        while(nSize > 0)
        {
            if(pBuf[0] == 0 && pBuf[1] == 0 && pBuf[2] == 1)
            {
                (*pAfterStartCodeIdx) += 3; //so that buf[0] is the actual data, not start code
                return 0;
            }
            ++pBuf;
            ++(*pAfterStartCodeIdx);
            --nSize;
        }
    }
    return -1;

}

static inline int supplyStreamData(SbmFrameAvc* pSbm)
{
    DetectFramePicInfo* pDetectInfo = &pSbm->mDetectInfo;

    if(pDetectInfo->pCurStream)
    {
        flushStream(pSbm, pDetectInfo->pCurStream, 0);
        pDetectInfo->pCurStream = NULL;
        if(pDetectInfo->pCurFramePic)
        {
           pDetectInfo->pCurFramePic->nlength += pDetectInfo->nCurStreamDataSize;
        }
        pDetectInfo->nCurStreamDataSize = 0;
        pDetectInfo->pCurStreamDataptr  = NULL;
    }

    VideoStreamDataInfo *pStream = requestStream(pSbm);
    if(pStream == NULL)
    {
        SemTimedWait(&pSbm->streamDataSem, 20);
        return -1;
    }
    pDetectInfo->pCurStream = pStream;
    pDetectInfo->nCurStreamDataSize = pDetectInfo->pCurStream->nLength;

    selectCheckBuffer(pSbm, pStream, &pDetectInfo->pCurStreamDataptr);

    pDetectInfo->nCurStreamRebackFlag = 0;
    if(pSbm->mConfig.bSecureVideoFlag == 0)
    {
       int nRingBufSize = pSbm->pStreamBufferEnd - pDetectInfo->pCurStream->pData + 1;
       if(pDetectInfo->pCurStream->nLength > nRingBufSize)
           pDetectInfo->nCurStreamRebackFlag = 1;
    }
    return 0;

}

static void disposeInvalidStreamData(SbmFrameAvc* pSbm)
{
    DetectFramePicInfo* pDetectInfo = &pSbm->mDetectInfo;

    int bNeedAddFramePic = 0;
    char* pStartBuf = NULL;

    if(pSbm->mConfig.bSecureVideoFlag == 1)
    {
        pStartBuf = pSbm->pTmpBuffer;
    }
    else
    {
        pStartBuf = pDetectInfo->pCurStream->pData;
    }

    logd("**1 pCurFramePic->nlength = %d, flag = %d",pDetectInfo->pCurFramePic->nlength,
         (pDetectInfo->pCurStreamDataptr == pStartBuf));

    if(pDetectInfo->pCurStreamDataptr == pStartBuf
       && pDetectInfo->pCurFramePic->nlength == 0)
    {
        pDetectInfo->pCurFramePic->pDataStartAddr = pDetectInfo->pCurStream->pData;
        pDetectInfo->pCurFramePic->nlength = pDetectInfo->pCurStream->nLength;
        pDetectInfo->pCurFramePic->bValidFlag = 0;
        bNeedAddFramePic = 1;
    }
    else
    {
        pDetectInfo->pCurFramePic->nlength += pDetectInfo->nCurStreamDataSize;
        logd("**2, pCurFramePic->nlength = %d, diff = %d",pDetectInfo->pCurFramePic->nlength,
             pDetectInfo->pCurFramePic->nlength - MAX_INVALID_STREAM_DATA_SIZE);

        if(pDetectInfo->pCurFramePic->nlength > MAX_INVALID_STREAM_DATA_SIZE)
        {
            pDetectInfo->pCurFramePic->bValidFlag = 0;
            bNeedAddFramePic = 1;
        }
    }

    logd("disposeInvalidStreamData: bNeedAddFramePic = %d",bNeedAddFramePic);
    flushStream(pSbm, pDetectInfo->pCurStream, 0);
    pDetectInfo->pCurStream = NULL;
    pDetectInfo->pCurStreamDataptr = NULL;
    pDetectInfo->nCurStreamDataSize = 0;

    if(bNeedAddFramePic)
    {
        addFramePic(pSbm, pDetectInfo->pCurFramePic);
        pDetectInfo->pCurFramePic = NULL;
    }

}

static inline void skipCurStreamDataBytes(SbmFrameAvc* pSbm, int nSkipSize)
{
    char* pStart = pSbm->pDetectBufStart;
    char* pEnd   = pSbm->pDetectBufEnd;
    DetectFramePicInfo* pDetectInfo = &pSbm->mDetectInfo;

    pDetectInfo->pCurStreamDataptr += nSkipSize;
    pDetectInfo->nCurStreamDataSize -= nSkipSize;
    if(pDetectInfo->pCurStreamDataptr > pEnd)
    {
        pDetectInfo->pCurStreamDataptr = pStart + (pDetectInfo->pCurStreamDataptr - pEnd - 1);
    }
    pDetectInfo->pCurFramePic->nlength += nSkipSize;

}

static inline void storeNaluInfo(SbmFrameAvc* pSbm, int nNaluType,
                                    int nNalRefIdc, int nNaluSize, char* pNaluBuf)
{
    DetectFramePicInfo* pDetectInfo = &pSbm->mDetectInfo;

    int nNaluIdx = pDetectInfo->pCurFramePic->nCurNaluIdx;
    NaluInfo* pNaluInfo = &pDetectInfo->pCurFramePic->pNaluInfoList[nNaluIdx];
    logv("*** nNaluIdx = %d, pts = %lld, naluSize = %d, nalutype = %d",
          nNaluIdx, pDetectInfo->pCurStream->nPts, nNaluSize, nNaluType);
    if(nNaluIdx < MAX_FRAME_PTS_LIST_NUM)
        pDetectInfo->nCurFramePtsList[nNaluIdx] = pDetectInfo->pCurStream->nPts;

    pNaluInfo->nNalRefIdc = nNalRefIdc;
    pNaluInfo->nType = nNaluType;
    pNaluInfo->pDataBuf = pNaluBuf;
    pNaluInfo->nDataSize = nNaluSize;
    pDetectInfo->pCurFramePic->nCurNaluIdx++;
    if(pDetectInfo->pCurFramePic->nCurNaluIdx >= pDetectInfo->pCurFramePic->nMaxNaluNum)
    {
        expandNaluList(pDetectInfo->pCurFramePic);
    }
    if(pSbm->mDetectInfo.pCurStream->bVideoInfoFlag == 1)
    {
        pDetectInfo->pCurFramePic->bVideoInfoFlag = 1;
    }
}

static char* checkCurStreamDataAddr(SbmFrameAvc* pSbm)
{
    DetectFramePicInfo* pDetectInfo = &pSbm->mDetectInfo;
    char* pCurStreamDataAddr = NULL;

    int nSize = pDetectInfo->pCurStreamDataptr - pSbm->pDetectBufStart;
    int nRingBufSize = pSbm->pStreamBufferEnd - pDetectInfo->pCurStream->pData + 1;

    if(nSize >= nRingBufSize)
    {
        logd("*** buffer ring ");
        pCurStreamDataAddr = pSbm->pStreamBuffer + (nSize - nRingBufSize);
    }
    else
        pCurStreamDataAddr = pDetectInfo->pCurStream->pData + nSize;

    return pCurStreamDataAddr;
}

/*
detect step:
    1. request bit stream
    2. find startCode
    3.  read the naluType and bFirstSliceSegment
    4. skip nAfterStartCodeIdx
    5. find the next startCode to determine size of cur nalu
    6. store  nalu info
    7. skip naluSize bytes
*/
static void detectWithStartCode(SbmFrameAvc* pSbm)
{
    char tmpBuf[6] = {0};
    char* pStart = pSbm->pDetectBufStart;
    char* pEnd   = pSbm->pDetectBufEnd;
    int   bFirstSliceSegment = 0;
    char* pAfterStartCodeBuf = NULL;
    DetectFramePicInfo* pDetectInfo = &pSbm->mDetectInfo;

    if(pDetectInfo->pCurFramePic == NULL)
    {
       pDetectInfo->pCurFramePic = requestEmptyFrame(pSbm);
       if(pDetectInfo->pCurFramePic == NULL)
       {
            SemTimedWait(&pSbm->emptyFramePicSem, 20);
            return ;
       }
       initFramePicInfo(pDetectInfo);
    }

    while(1)
    {
        int   nRet = -1;

        //*1. request bit stream
        if(pDetectInfo->nCurStreamDataSize < 5 || pDetectInfo->pCurStreamDataptr == NULL)
        {
            if(supplyStreamData(pSbm) != 0)
            {
                if(pDetectInfo->bCurFrameStartCodeFound == 1 && pSbm->nEosFlag == 1)
                {
                    pDetectInfo->bCurFrameStartCodeFound = 0;
                    chooseFramePts(pDetectInfo);
                    addFramePic(pSbm, pDetectInfo->pCurFramePic);
                    logd("find eos, flush last stream frame, pts = %lld",
                          (long long int)pDetectInfo->pCurFramePic->nPts);
                    pDetectInfo->pCurFramePic = NULL;
                }
                return ;
            }
        }

        logv("*** new new pSbm->mConfig.bSecureVideoFlag = %d",pSbm->mConfig.bSecureVideoFlag);
        if(pDetectInfo->pCurFramePic->pDataStartAddr == NULL)
        {
           if(pSbm->mConfig.bSecureVideoFlag == 1)
           {
                pDetectInfo->pCurFramePic->pDataStartAddr = checkCurStreamDataAddr(pSbm);
           }
           else
           {
                pDetectInfo->pCurFramePic->pDataStartAddr = pDetectInfo->pCurStreamDataptr;
           }
           pDetectInfo->pCurFramePic->pVideoInfo = pDetectInfo->pCurStream->pVideoInfo;
        }

        //*2. find startCode
        int nAfterStartCodeIdx = 0;
        nRet = searchStartCode(pSbm,&nAfterStartCodeIdx);
        if(nRet != 0 //*  can not find startCode
           || pDetectInfo->pCurFramePic->nCurNaluIdx > MAX_NALU_NUM_IN_FRAME)
        {
            logw("can not find startCode, curNaluIdx = %d, max = %d",
                  pDetectInfo->pCurFramePic->nCurNaluIdx, MAX_NALU_NUM_IN_FRAME);
            disposeInvalidStreamData(pSbm);
            return ;
        }

        //* now had find the startCode
        //*3.  read the naluType and bFirstSliceSegment

        pAfterStartCodeBuf = pDetectInfo->pCurStreamDataptr + nAfterStartCodeIdx;
        tmpBuf[0] = readByteIdx(pAfterStartCodeBuf ,pStart, pEnd, 0);

        int nNalRefIdc = (tmpBuf[0]&0x60)?1:0;
        int nNaluType = tmpBuf[0] & 0x1f;
        char* pSliceStartPlusOne = pAfterStartCodeBuf + 1;

        if(pSliceStartPlusOne > pEnd)
            pSliceStartPlusOne -= (pEnd - pStart + 1);

#if ADD_ONE_FRAME
        if((nNaluType >= 6 && nNaluType <= 13) || (nNaluType >= 15 && nNaluType < 20)
             || (nNaluType >= 21 && nNaluType <= 31))

        {
            /* Begining of access unit, needn't bFirstSliceSegment */
            if(pDetectInfo->bCurFrameStartCodeFound == 1)
            {
                pDetectInfo->bCurFrameStartCodeFound = 0;
                chooseFramePts(pDetectInfo);
                addFramePic(pSbm, pDetectInfo->pCurFramePic);
                pDetectInfo->pCurFramePic = NULL;
                return ;
            }
        }

        if(IsFrameNalu(nNaluType))
        {
            u32 n = 0;
            if(nNaluType == 20 || nNaluType == 14)
                n = 24;
            bFirstSliceSegment = readGolomb(pSliceStartPlusOne, &n);

            logv("***bFirstSliceSegment = %d", bFirstSliceSegment);

            if(!bFirstSliceSegment)//=0
            {
                if(pDetectInfo->bCurFrameStartCodeFound == 0)
                {
                    pDetectInfo->bCurFrameStartCodeFound = 1;
                    pDetectInfo->pCurFramePic->nFrameNaluType = nNaluType;
                }
                else
                {
                    logv("**** have found one frame pic ****");
                    pDetectInfo->bCurFrameStartCodeFound = 0;
                    chooseFramePts(pDetectInfo);
                    addFramePic(pSbm, pDetectInfo->pCurFramePic);
                    pDetectInfo->pCurFramePic = NULL;
                    return ;
                }
            }
        }
#else
        if(pDetectInfo->bCurFrameStartCodeFound == 0)
        {
            pDetectInfo->bCurFrameStartCodeFound = 1;
            pDetectInfo->pCurFramePic->nFrameNaluType = nNaluType;
        }
        else
        {
            logv("**** have found one frame pic ****");
            pDetectInfo->bCurFrameStartCodeFound = 0;
            chooseFramePts(pDetectInfo);
            addFramePic(pSbm, pDetectInfo->pCurFramePic);
            pDetectInfo->pCurFramePic = NULL;
            return ;
        }
#endif

        //*if code run here, it means that this is the normal nalu of new frame, we should store it
        //*4. skip nAfterStartCodeIdx
        skipCurStreamDataBytes(pSbm, nAfterStartCodeIdx);

        //*5. find the next startCode to determine size of cur nalu
        int nNaluSize = 0;
        nAfterStartCodeIdx = 0;
        nRet = searchStartCode(pSbm,&nAfterStartCodeIdx);
        if(nRet != 0)//* can not find next startCode
        {
            nNaluSize = pDetectInfo->nCurStreamDataSize;
        }
        else
        {
            nNaluSize = nAfterStartCodeIdx - 3; //* 3 is the length of startCode

        }

        logv("gqy***nNaluType =%d, store nalu size = %d, nNalRefIdc = %d",
               nNaluType, nNaluSize, nNalRefIdc);
        //*6. store  nalu info
        char* pNaluBuf = NULL;
        if(pSbm->mConfig.bSecureVideoFlag == 1)
        {
            pNaluBuf = checkCurStreamDataAddr(pSbm);
        }
        else
            pNaluBuf = pDetectInfo->pCurStreamDataptr;

        storeNaluInfo(pSbm, nNaluType, nNalRefIdc, nNaluSize, pNaluBuf);

        //*7. skip naluSize bytes
        skipCurStreamDataBytes(pSbm, nNaluSize);
    }

    return ;
}


/*
detect step:
    1. request bit stream
    2. read nalu size
    3. read the naluType and bFirstSliceSegment
    4. skip 4 bytes
    5. store  nalu info
    6. skip naluSize bytes
*/
static void detectWithoutStartCode(SbmFrameAvc* pSbm)
{
    char tmpBuf[6] = {0};
    char* pStart = pSbm->pDetectBufStart;
    char* pEnd   = pSbm->pDetectBufEnd;
    char* pAfterStartCodeBuf = NULL;
    unsigned int bFirstSliceSegment = 0;
    const int nPrefixBytes = 4; // indicate data length
    DetectFramePicInfo* pDetectInfo = &pSbm->mDetectInfo;
    if(pDetectInfo->pCurFramePic == NULL)
    {
       pDetectInfo->pCurFramePic = requestEmptyFrame(pSbm);
       if(pDetectInfo->pCurFramePic == NULL)
       {
            SemTimedWait(&pSbm->emptyFramePicSem, 20);
            return ;
       }
       //usleep(10*1000);
       initFramePicInfo(pDetectInfo);
    }

    while(1)
    {
        int   nNaluSize = 0;
        unsigned int nNalRefIdc = 0;
        unsigned int nNaluType = 0;

        //*1. request bit stream
        if(pDetectInfo->nCurStreamDataSize < 5 || pDetectInfo->pCurStreamDataptr == NULL)
        {
            if(supplyStreamData(pSbm) != 0)
            {
                if(pDetectInfo->bCurFrameStartCodeFound == 1 && pSbm->nEosFlag == 1)
                {
                    pDetectInfo->bCurFrameStartCodeFound = 0;
                    chooseFramePts(pDetectInfo);
                    addFramePic(pSbm, pDetectInfo->pCurFramePic);
                    pDetectInfo->pCurFramePic = NULL;
                }
                return ;
            }
        }

        if(pDetectInfo->pCurFramePic->pDataStartAddr == NULL)
        {
           if(pSbm->mConfig.bSecureVideoFlag == 1)
           {
                pDetectInfo->pCurFramePic->pDataStartAddr = checkCurStreamDataAddr(pSbm);
           }
           else
           {
                pDetectInfo->pCurFramePic->pDataStartAddr = pDetectInfo->pCurStreamDataptr;
           }
           pDetectInfo->pCurFramePic->pVideoInfo     = pDetectInfo->pCurStream->pVideoInfo;
        }

        //*2. read nalu size
        tmpBuf[0] = readByteIdx(pDetectInfo->pCurStreamDataptr ,pStart,pEnd, 0);
        tmpBuf[1] = readByteIdx(pDetectInfo->pCurStreamDataptr ,pStart,pEnd, 1);
        tmpBuf[2] = readByteIdx(pDetectInfo->pCurStreamDataptr ,pStart,pEnd, 2);
        tmpBuf[3] = readByteIdx(pDetectInfo->pCurStreamDataptr ,pStart,pEnd, 3);
        nNaluSize = (tmpBuf[0] << 24) | (tmpBuf[1] << 16) | (tmpBuf[2] << 8) | tmpBuf[3];
        if(nNaluSize > (pDetectInfo->nCurStreamDataSize - nPrefixBytes)
           || nNaluSize == 0
           || pDetectInfo->pCurFramePic->nCurNaluIdx > MAX_NALU_NUM_IN_FRAME)
        {
            logw(" error: nNaluSize[%u] > nCurStreamDataSize[%d], curNaluIdx = %d, max = %d",
                   nNaluSize, pDetectInfo->nCurStreamDataSize,
                   pDetectInfo->pCurFramePic->nCurNaluIdx, MAX_NALU_NUM_IN_FRAME);
            disposeInvalidStreamData(pSbm);
            return ;
        }

        //*3. read the naluType and bFirstSliceSegment

        pAfterStartCodeBuf = pDetectInfo->pCurStreamDataptr + nPrefixBytes;
        tmpBuf[0] = readByteIdx(pAfterStartCodeBuf ,pStart, pEnd, 0);

        nNalRefIdc = (tmpBuf[0]&0x60)?1:0;
        nNaluType = tmpBuf[0] & 0x1f;
        char* pSliceStartPlusOne = pAfterStartCodeBuf + 1;

        if(pSliceStartPlusOne > pEnd)
        {
            pSliceStartPlusOne -= (pEnd - pStart + 1);
        }


        logv("*** nNaluType = %d, nNalRefIdc = %d",nNaluType, nNalRefIdc);

#if ADD_ONE_FRAME
        if((nNaluType >= 6 && nNaluType <= 13) || (nNaluType >= 15 && nNaluType < 20)
             || (nNaluType >= 21 && nNaluType <= 31))
        {
            /* Begining of access unit, needn't bFirstSliceSegment */
            if(pDetectInfo->bCurFrameStartCodeFound == 1)
            {
                pDetectInfo->bCurFrameStartCodeFound = 0;
                chooseFramePts(pDetectInfo);
                addFramePic(pSbm, pDetectInfo->pCurFramePic);
                pDetectInfo->pCurFramePic = NULL;
                return ;
            }
        }

        if(IsFrameNalu(nNaluType))
        {
            u32 n = 0;
            if(nNaluType == 20 || nNaluType == 14)
                n = 24;
            bFirstSliceSegment = readGolomb(pSliceStartPlusOne, &n);

            if(!bFirstSliceSegment)//=0
            {
                if(pDetectInfo->bCurFrameStartCodeFound == 0)
                {
                    pDetectInfo->bCurFrameStartCodeFound = 1;
                    pDetectInfo->pCurFramePic->nFrameNaluType = nNaluType;
                }
                else
                {
                    logv("**** have found one frame pic ****");
                    pDetectInfo->bCurFrameStartCodeFound = 0;
                    chooseFramePts(pDetectInfo);
                    addFramePic(pSbm, pDetectInfo->pCurFramePic);
                    pDetectInfo->pCurFramePic = NULL;
                    return ;
                }
            }
        }
#else
        if(pDetectInfo->bCurFrameStartCodeFound == 0)
        {
            pDetectInfo->bCurFrameStartCodeFound = 1;
            pDetectInfo->pCurFramePic->nFrameNaluType = nNaluType;
        }
        else
        {
            logv("**** have found one frame pic ****");
            pDetectInfo->bCurFrameStartCodeFound = 0;
            chooseFramePts(pDetectInfo);
            addFramePic(pSbm, pDetectInfo->pCurFramePic);
            pDetectInfo->pCurFramePic = NULL;
            return ;
        }
#endif

        //*4. skip 4 bytes
        skipCurStreamDataBytes(pSbm, nPrefixBytes);
        logv("gqy***nNaluType =%d, store nalu size = %d, nNalRefIdc = %d",
             nNaluType, nNaluSize, nNalRefIdc);

        //*5. store  nalu info
        char* pNaluBuf = NULL;
        if(pSbm->mConfig.bSecureVideoFlag == 1)
        {
            pNaluBuf = checkCurStreamDataAddr(pSbm);
        }
        else
            pNaluBuf = pDetectInfo->pCurStreamDataptr;

        storeNaluInfo(pSbm, nNaluType, nNalRefIdc, nNaluSize, pNaluBuf);

        //*6. skip naluSize bytes
        skipCurStreamDataBytes(pSbm, nNaluSize);
    }
    return ;
}

static void detectOneFramePic(SbmFrameAvc* pSbm)
{
    if(pSbm->bStreamWithStartCode == 1)
    {
        detectWithStartCode(pSbm);
    }
    else
    {
        detectWithoutStartCode(pSbm);
    }

}


static void* ProcessThread(void* pThreadData)
{
    SbmFrameAvc* pSbm = (SbmFrameAvc*)pThreadData;
    DetectFramePicInfo* pDetectInfo = &pSbm->mDetectInfo;
    CdcMessage msg;
    memset(&msg, 0, sizeof(CdcMessage));

    while(1)
    {
        if(CdcMessageQueueTryGetMessage(pSbm->mq, &msg, 20) == 0)
        {
            //* process message here
            if(msg.messageId == SBM_THREAD_CMD_QUIT)
            {
                goto EXIT;
            }
            else if(msg.messageId == SBM_THREAD_CMD_READ)
            {
                if(pSbm->bStreamWithStartCode == -1)
                {
                    checkBitStreamType(pSbm);
                }
                else
                {
                    detectOneFramePic(pSbm);
                }

                if(CdcMessageQueueGetCount(pSbm->mq) <= 0)
                {
                    msg.messageId = SBM_THREAD_CMD_READ;
                    CdcMessageQueuePostMessage(pSbm->mq, &msg);
                }
            }
            else if(msg.messageId == SBM_THREAD_CMD_RESET)
            {
                logd("*** post reset sem");
                if(pDetectInfo->pCurStream)
                {
                    flushStream(pSbm, pDetectInfo->pCurStream, 0);
                    pDetectInfo->pCurStream = NULL;
                }

                lock(pSbm);

                pDetectInfo->bCurFrameStartCodeFound = 0;
                pDetectInfo->nCurStreamDataSize = 0;
                pDetectInfo->nCurStreamRebackFlag = 0;
                pDetectInfo->pCurStreamDataptr = NULL;

                if(pDetectInfo->pCurFramePic)
                {
                   pDetectInfo->pCurFramePic = NULL;
                }

                pSbm->pWriteAddr                 = pSbm->pStreamBuffer;
                pSbm->nValidDataSize             = 0;

                pSbm->frameFifo.nReadPos         = 0;
                pSbm->frameFifo.nWritePos        = 0;
                pSbm->frameFifo.nFlushPos        = 0;
                pSbm->frameFifo.nValidFrameNum   = 0;
                pSbm->frameFifo.nUnReadFrameNum  = 0;

                pSbm->mFramePicFifo.nFPFlushPos = 0;
                pSbm->mFramePicFifo.nFPReadPos  = 0;
                pSbm->mFramePicFifo.nFPWritePos = 0;
                pSbm->mFramePicFifo.nUnReadFramePicNum = 0;
                pSbm->mFramePicFifo.nValidFramePicNum = 0;

                unlock(pSbm);

                sem_t* replySem = (sem_t*)msg.params[0];
                sem_post(replySem);
                //* do nothing
            }
        }

    }

EXIT:
    logd(" exit sbm thread ");
    return NULL;

}

SbmInterface* GetSbmInterfaceFrameAvc()
{
    logd("******* sbm-type: Frame_AVC*******");
    SbmFrameAvc* pSbmFrameAvc = NULL;
    pSbmFrameAvc = (SbmFrameAvc*)malloc(sizeof(SbmFrameAvc));
    if(pSbmFrameAvc == NULL)
    {
        loge("malloc for sbm frame struct failed");
        return NULL;
    }

    memset(pSbmFrameAvc, 0, sizeof(SbmFrameAvc));

    pSbmFrameAvc->sbmInterface.init    = SbmFrameAvcInit;
    pSbmFrameAvc->sbmInterface.destroy = SbmFrameAvcDestroy;
    pSbmFrameAvc->sbmInterface.reset   = SbmFrameAvcReset;
    pSbmFrameAvc->sbmInterface.getBufferSize         = SbmFrameAvcGetBufferSize;
    pSbmFrameAvc->sbmInterface.getStreamDataSize     = SbmFrameAvcGetStreamDataSize;
    pSbmFrameAvc->sbmInterface.getStreamFrameNum     = SbmFrameAvcGetStreamFrameNum;
    pSbmFrameAvc->sbmInterface.getBufferAddress      = SbmFrameAvcGetBufferAddress;
    pSbmFrameAvc->sbmInterface.getBufferWritePointer = SbmFrameAvcGetBufferWritePointer;
    pSbmFrameAvc->sbmInterface.getBufferDataInfo = SbmFrameAvcGetBufferDataInfo;
    pSbmFrameAvc->sbmInterface.requestBuffer     = SbmFrameAvcRequestBuffer;
    pSbmFrameAvc->sbmInterface.addStream         = SbmFrameAvcAddStream;
    pSbmFrameAvc->sbmInterface.requestStream     = SbmFrameAvcRequestFramePic;
    pSbmFrameAvc->sbmInterface.returnStream      = SbmFrameAvcReturnFramePic;
    pSbmFrameAvc->sbmInterface.flushStream       = SbmFrameAvcFlushFramePic;
    pSbmFrameAvc->sbmInterface.setEos            = SbmFrameAvcSetEos;

    pSbmFrameAvc->sbmInterface.nType = SBM_TYPE_FRAME_AVC;

    return &pSbmFrameAvc->sbmInterface;
}

