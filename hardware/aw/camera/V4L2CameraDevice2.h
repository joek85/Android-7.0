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

#ifndef __HAL_V4L2_CAMERA_DEVICE_H__
#define __HAL_V4L2_CAMERA_DEVICE_H__

/*
 * Contains declaration of a class V4L2CameraDevice.
 */

#include <ui/Rect.h>
#include <fcntl.h>
#include <utils/Thread.h>
#include <hardware/camera.h>
#include <camerahal/linux/videodev2_new.h>
#include<sys/stat.h>
#include <hal_public.h> //GPU dependencies


#include <camerahal/type_camera.h>

#include "OSAL_Queue.h"
#include "Libve_Decoder2.h"

//#include "Libve_Decoder.h"

#define NB_BUFFER 10

#define MAX_PICTURE_SIZE (4608*3456*3>>1)
#define DISCARD_FRAME_NUM 1
#if 1 //jiangwei
#define CLEAR(x) (memset(&(x), 0, sizeof (x)))
#endif



namespace android {

class CameraHardware;
class CallbackNotifier;
class PreviewWindow;

/*
 *
 */
typedef struct HALCameraInfo{
    char    device_name[16];        // device node name, such as "/dev/video0"
    int     device_id;              // device id for camera share with the same CSI
    int     facing;                 // facing front or back
    int     orientation;            //
    bool    fast_picture_mode;      //
    bool    is_uvc;                 // usb camera
}HALCameraInfo;

enum TAKE_PICTURE_STATE {
    // do not take picture
    TAKE_PICTURE_NULL,
    // stream off -> stream on -> take picture -> stream off -> stream on
    TAKE_PICTURE_NORMAL,
    // normal mode but do not need to stream off/on
    TAKE_PICTURE_FAST,
    // take picture in recording
    TAKE_PICTURE_RECORD,
    TAKE_PICTURE_CONTINUOUS,
    TAKE_PICTURE_CONTINUOUS_FAST,
    // take smart picture
    TAKE_PICTURE_SMART,
};

/*
 * V4L2CameraDevice
 */
class V4L2CameraDevice {
public:
    /* Constructs V4L2CameraDevice instance. */
    V4L2CameraDevice(CameraHardware* camera_hal,
                     PreviewWindow * preview_window,
                     CallbackNotifier * cb);

    /* Destructs V4L2CameraDevice instance. */
    ~V4L2CameraDevice();

    /***************************************************************************
     * V4L2Camera device abstract interface implementation.
     * See declarations of these methods in V4L2Camera class for
     * information on each of these methods.
     **************************************************************************/

public:
    /* Connects to the camera device.
     * Since there is no real device to connect to, this method does nothing,
     * but changes the state.
     */
    status_t connectDevice(HALCameraInfo * halInfo);

    /* Disconnects from the camera device.
     * Since there is no real device to disconnect from, this method does
     * nothing, but changes the state.
     */
    status_t disconnectDevice();

    /* Starts the camera device. */
    status_t startDevice(int width, int height, uint32_t pix_fmt, bool video_hint);

    /* Stops the camera device. */
    status_t stopDevice();

    /* Gets current preview fame into provided buffer. */
    status_t getPreviewFrame(void* buffer);

    /* Starts delivering frames captured from the camera device.
     * This method will start the worker thread that would be pulling frames from
     * the camera device, and will deliver the pulled frames back to the emulated
     * camera via onNextFrameAvailable callback. This method must be called on a
     * connected instance of this class with a started camera device. If it is
     * called on a disconnected instance, or camera device has not been started,
     * this method must return a failure.
     * Param:
     *  one_burst - Controls how many frames should be delivered. If this
     *      parameter is 'true', only one captured frame will be delivered to the
     *      V4L2Camera. If this parameter is 'false', frames will keep
     *      coming until stopDeliveringFrames method is called. Typically, this
     *      parameter is set to 'true' only in order to obtain a single frame
     *      that will be used as a "picture" in takePicture method of the
     *      V4L2Camera.
     * Return:
     *  NO_ERROR on success, or an appropriate error status.
     */
    status_t startDeliveringFrames();

    /* Stops delivering frames captured from the camera device.
     * This method will stop the worker thread started by startDeliveringFrames.
     * Return:
     *  NO_ERROR on success, or an appropriate error status.
     */
    status_t stopDeliveringFrames();

    /***************************************************************************
     * Worker thread management overrides.
     * See declarations of these methods in V4L2Camera class for
     * information on each of these methods.
     **************************************************************************/

protected:

    // -------------------------------------------------------------------------
    // extended interfaces here <***** star *****>
    // -------------------------------------------------------------------------

    class DoCaptureThread : public Thread {
        V4L2CameraDevice*    mV4l2CameraDevice;
        bool                mRequestExit;
    public:
        DoCaptureThread(V4L2CameraDevice* dev) :
            Thread(false),
            mV4l2CameraDevice(dev),
            mRequestExit(false) {
        }
        void startThread() {
            run("CameraCaptureThread", PRIORITY_URGENT_DISPLAY);
        }
        void stopThread() {
            mRequestExit = true;
        }
        virtual bool threadLoop() {
            if (mRequestExit) {
                return false;
            }
            return mV4l2CameraDevice->captureThread();
        }
    };

    class DoPreviewThread : public Thread {
        V4L2CameraDevice*    mV4l2CameraDevice;
        bool                mRequestExit;
    public:
        DoPreviewThread(V4L2CameraDevice* dev) :
            Thread(false),
            mV4l2CameraDevice(dev),
            mRequestExit(false) {
        }
        void startThread() {
            run("CameraPreviewThread", PRIORITY_URGENT_DISPLAY);
        }
        void stopThread() {
            mRequestExit = true;
        }
        virtual bool threadLoop() {
            if (mRequestExit) {
                return false;
            }
            return mV4l2CameraDevice->previewThread();
        }
    };

    class DoPictureThread : public Thread {
        V4L2CameraDevice*    mV4l2CameraDevice;
        bool                mRequestExit;
    public:
        DoPictureThread(V4L2CameraDevice* dev) :
            Thread(false),
            mV4l2CameraDevice(dev),
            mRequestExit(false) {
        }
        void startThread() {
            run("CameraPictrueThread", PRIORITY_URGENT_DISPLAY);
        }
        void stopThread() {
            mRequestExit = true;
        }
        virtual bool threadLoop() {
            if (mRequestExit) {
                return false;
            }
            return mV4l2CameraDevice->pictureThread();
        }
    };

    class DoContinuousPictureThread : public Thread {
        V4L2CameraDevice*    mV4l2CameraDevice;
        bool                mRequestExit;
    public:
        DoContinuousPictureThread(V4L2CameraDevice* dev) :
            Thread(false),
            mV4l2CameraDevice(dev),
            mRequestExit(false) {
        }
        void startThread() {
            run("CameraContinuousPictrueThread", PRIORITY_URGENT_DISPLAY);
        }
        void stopThread() {
            mRequestExit = true;
        }
        virtual bool threadLoop() {
            if (mRequestExit) {
                return false;
            }
            return mV4l2CameraDevice->continuousPictureThread();
        }
    };

    class DoSmartPictureThread : public Thread {
        V4L2CameraDevice*    mV4l2CameraDevice;
        bool                mRequestExit;
    public:
        DoSmartPictureThread(V4L2CameraDevice* dev) :
            Thread(false),
            mV4l2CameraDevice(dev),
            mRequestExit(false) {
        }
        void startThread() {
            run("CameraSmartPictrueThread", PRIORITY_URGENT_DISPLAY);
        }
        void stopThread() {
            mRequestExit = true;
        }
        virtual bool threadLoop() {
            if (mRequestExit) {
                return false;
            }
            return mV4l2CameraDevice->smartPictureThread();
        }
    };

public:

    bool captureThread();
    bool previewThread();
    bool pictureThread();
    bool continuousPictureThread();
    bool smartPictureThread();

    int tryFmt(int format);                            // check if driver support this format
    int tryFmtSize(int * width, int * height);        // check if driver support this size
    int setFrameRate(int rate);                        // set frame rate from camera.cfg
    int getFrameRate();                                // get v4l2 device current frame rate

    int enumSize(char * pSize, int len);
    int getFullSize(int * full_w, int * full_h);
    int getSuitableThumbScale(int full_w, int full_h);

    int setImageEffect(int effect);
    int setWhiteBalance(int wb);

    int setTakePictureCtrl();

    // exposure
    int setExposureMode(int mode);
    int setExposureCompensation(int val);
    int setExposureWind(int num, void *areas);

    int setFlashMode(int mode);

    // af
    int setAutoFocusInit();
    int setAutoFocusRelease();
    int setAutoFocusRange(int range);
    int setAutoFocusWind(int num, void *areas);
    int setAutoFocusStart();
    int setAutoFocusStop();
    int getAutoFocusStatus();

    int getSnrValue();
    int set3ALock(int lock);

    void releasePreviewFrame(int index);            // Q buffer for encoder

    bool checkBufferState(int releaseBufferIndex)
    {
        if(mbufferUnit[releaseBufferIndex].buffer_state)
        {
	     mbufferUnit[releaseBufferIndex].buffer_state = false;
            return true;
        }
        return false;
    }

    void refreshBufferState(int releaseBufferIndex,bool state)
    {
        //false :camera hal hold the buffer
        //true:frameworks hold the buffer
        mbufferUnit[releaseBufferIndex].buffer_state = state;
    }


    void clearBuffer(void)
    {
        int i = 0;
        int ret = -1;

        for(i = 0; i <NB_BUFFER;i++)
        {
            if(mbufferUnit[i].buffer_state)
            {
                mbufferUnit[i].buffer_state = false;

                releasePreviewFrame(i);

                //mbufferUnit[i].delete_handle_state = true;
                 LOGV("zjw,in stop recoding we find some buffer should be released,buffer index = %d,base = 0x%x ",i,((private_handle_t*)(mbufferUnit[i].mbufferUnitMetadata))->base);
            }
        }
        LOGV("zjw,in stop recoding we wanna to clearBuffer\n");
    }

    int getCurrentFaceFrame(void * frame);

    void setThumbUsedForVideo(bool isThumbUsedForVideo)
    {
        mIsThumbUsedForVideo = isThumbUsedForVideo;
    }

    void setVideoSize(int w, int h)
    {
        mVideoWidth = w;
        mVideoHeight = h;
    }

    inline void setCrop(int new_zoom, int max_zoom)
    {
        mNewZoom = new_zoom;
        mMaxZoom = max_zoom;
    }

    inline int getCaptureFormat()
    {
        return mCaptureFormat;
    }

    inline void setHwEncoder(bool hw)
    {
        mUseHwEncoder = hw;
    }

    inline void setTakePictureState(TAKE_PICTURE_STATE state)
    {
        pthread_mutex_lock(&mCaptureMutex);
        LOGV("setTakePictureState %d", state);
        mTakePictureState = state;
        pthread_mutex_unlock(&mCaptureMutex);
    }

    void startContinuousPicture();
    void stopContinuousPicture();
    void setContinuousPictureCnt(int cnt);

    void startSmartPicture();
    void stopSmartPicture();

    /*
     * State checkers.
     */
    inline bool isConnected() const {
        /* Instance is connected when its status is either"connected", or
         * "started". */
        return mCameraDeviceState == STATE_CONNECTED || mCameraDeviceState == STATE_STARTED;
    }
    inline bool isStarted() const {
        return mCameraDeviceState == STATE_STARTED;
    }

private:
    int openCameraDev(HALCameraInfo * halInfo);
    void closeCameraDev();
    int v4l2SetVideoParams(int width, int height, uint32_t pix_fmt);
    int v4l2setCaptureParams();
    int v4l2ReqBufs(int * buf_cnt);
    int v4l2QueryBuf();
    int v4l2StartStreaming();
    int v4l2StopStreaming();
    int v4l2UnmapBuf();

    int v4l2WaitCameraReady();
    int getPreviewFrame(v4l2_buffer *buf);

    void dealWithVideoFrameSW(V4L2BUF_t * pBuf);
    void dealWithVideoFrameHW(V4L2BUF_t * pBuf);
    void dealWithVideoFrameTest(V4L2BUF_t * pBuf);

    /* Checks if it's the time to push new frame to continuous picture.
     * Note that this method must be called while object is locked. */
    bool isContinuousPictureTime();

    bool isPreviewTime();

    void waitFaceDectectTime();

    void singalDisconnect();
public:

    void  showformat(int format, const char *str);
    // instance of PreviewWindow
    PreviewWindow *                    mPreviewWindow;

private:
    // -------------------------------------------------------------------------
    // private data
    // -------------------------------------------------------------------------

    /* Locks this instance for parameters, state, etc. change. */
    Mutex                           mObjectLock;

    // instance of CameraHardware
    CameraHardware *                 mCameraHardware;

    // instance of CallbackNotifier
    CallbackNotifier *                mCallbackNotifier;

    HALCameraInfo                    mHalCameraInfo;

    /* Defines possible states of the V4L2Camera device object.
     */
    enum CameraDeviceState {
        /* Object has been constructed. */
        STATE_CONSTRUCTED,
        /* Object has been connected to the physical device. */
        STATE_CONNECTED,
        /* Camera device has been started. */
        STATE_STARTED,
    };

    /* Object state. */
    CameraDeviceState               mCameraDeviceState;

    /* Defines possible states of the V4L2CameraDevice capture thread.
     */
    enum CaptureThreadState {
        /* Do not capture frame. */
        CAPTURE_STATE_NULL,
        /* Do not capture frame. */
        CAPTURE_STATE_PAUSED,
        /* Start capture frame. */
        CAPTURE_STATE_STARTED,
        /* exit thread*/
        CAPTURE_STATE_EXIT,
    };

    /* capture thread state. */
    CaptureThreadState                mCaptureThreadState;

    // v4l2 device handle
    int                                mCameraFd;

    // be usb camera or not
    bool                            mIsUsbCamera;

    int                                mFrameRate;

    TAKE_PICTURE_STATE                mTakePictureState;
    V4L2BUF_t                        mPicBuffer;
    bool                            mIsPicCopy;

    /* Frame width */
    int                             mFrameWidth;

    /* Frame height */
    int                             mFrameHeight;

    /* thumb Frame width */
    int                             mThumbWidth;

    /* thumb Frame height */
    int                             mThumbHeight;

    /* Timestamp of the current frame. */
    nsecs_t                         mCurFrameTimestamp;
#if 0

    typedef struct v4l2_mem_map_t{
        void *    mem[NB_BUFFER];
        int     length;
    }v4l2_mem_map_t;
    v4l2_mem_map_t                    mMapMem;
#endif
#if 1
typedef struct v4l2_mem_from_device_t{
       void *        mem[NB_BUFFER];
       int             length;
       int             nShareBufFd[NB_BUFFER];
       int             nDmaBufFd[NB_BUFFER];
}v4l2_mem_from_device_t;
v4l2_mem_from_device_t   mMapMem;
#endif
/* the number of pictrue-addr we can get from V4L2,the value rely on the macro we
set to V4L2,in v4l2SetVideoParams Parameter pix_fmt*/		
//1.nplanes = 1:we get yuv addr buf.m.planes[0]
//2.nplanes = 2:we get y addr from buf.m.planes[0] ,get uv addr from buf.m.planes[1]...
//3.nplanes = 3:we get y addr from buf.m.planes[0] ,get u addr from buf.m.planes[1],get v addr from buf.m.planes[2]...

    unsigned int nplanes;
    unsigned 	int lengthY;
    unsigned    int lengthUV;

    // actually buffer counts
    int                                mBufferCnt;

    bool                            mUseHwEncoder;

    Rect                            mRectCrop;
    Rect                            mThumbRectCrop;
    int                                mNewZoom;
    int                                mLastZoom;
    int                                mMaxZoom;

    // the driver capture format
    int                                mCaptureFormat;
    // the user request format, it should convert video buffer format
    // if mVideoFormat is different from mCaptureFormat.
    int                                mVideoFormat;

//TOOLS FOR CAPTURE BITSTREAM
#if DBG_CAPTURE_STREAM_FROM_V4L2DEVICE_TAKEPICTURE||DBG_CAPTURE_STREAM_FROM_V4L2DEVICE_FORNV21||DBG_CAPTURE_STREAM_FROM_V4L2DEVICE_FORYUYV||DBG_CAPTURE_STREAM_FROM_V4L2DEVICE_FORMJPEG||DBG_CAPTURE_STREAM_FROM_V4L2DEVICE_FORH264
    FILE *fp_stream_from_v4l2device = NULL;
    int v4l2device_tail_num= 0;
    int frame_num0 = 0;
#endif
#if DBG_CAPTURE_STREAM_AFTER_TRANSFORMATION
    FILE *fp_stream_after_transformation = NULL;
    int afterTransformation_tail_num = 0;
    int frame_num1 = 0;
#endif

    typedef struct bufferManagerQ_t
    {
        unsigned long            buf_vir_addr[NB_BUFFER];
        unsigned long            buf_phy_addr[NB_BUFFER];
        int            write_id;
        int            read_id;
        int            buf_unused;
	int nShareBufFd[NB_BUFFER];
    }bufferManagerQ_t;
    bufferManagerQ_t                mVideoBuffer;        // for usb camera


    typedef struct bufferUnit
    {
        //buffer_state = false :camera hal hold the buffer
        //buffer_state = true:frameworks hold the buffer
        bool buffer_state;
        //delete_handle_state :reserved
        bool delete_handle_state;
        private_handle_t* mbufferUnitMetadata;
    }bufferUnit;

    bufferUnit                               mbufferUnit[NB_BUFFER];

    OSAL_QUEUE                        mQueueBufferPreview;
    OSAL_QUEUE                        mQueueBufferPicture;
    V4L2BUF_t                        mV4l2buf[NB_BUFFER];

    sp<DoCaptureThread>                mCaptureThread;
    pthread_mutex_t                 mCaptureMutex;
    pthread_cond_t                    mCaptureCond;

    sp<DoPreviewThread>                mPreviewThread;
    pthread_mutex_t                 mPreviewMutex;
    pthread_cond_t                    mPreviewCond;

    sp<DoPictureThread>                mPictureThread;
    pthread_mutex_t                 mPictureMutex;
    pthread_cond_t                    mPictureCond;

    pthread_mutex_t                 mConnectMutex;
    pthread_cond_t                    mConnectCond;
    bool                            mCanBeDisconnected;

    sp<DoContinuousPictureThread>    mContinuousPictureThread;
    pthread_mutex_t                 mContinuousPictureMutex;
    pthread_cond_t                    mContinuousPictureCond;
    bool                            mContinuousPictureStarted;
    int                                mContinuousPictureCnt;
    int                                mContinuousPictureMax;
    nsecs_t                         mContinuousPictureStartTime;

    sp<DoSmartPictureThread>        mSmartPictureThread;
    pthread_mutex_t                 mSmartPictureMutex;
    pthread_cond_t                    mSmartPictureCond;
    bool                            mSmartPictureDone;

    /* Timestamp (abs. microseconds) when last frame has been pushed to the
     * preview window. */
    uint64_t                        mContinuousPictureLast;

    /* Preview frequency in microseconds. */
    uint32_t                        mContinuousPictureAfter;

    uint64_t                        mFaceDectectLast;
    uint32_t                        mFaceDectectAfter;

    uint64_t                        mPreviewLast;
    uint32_t                        mPreviewAfter;

    V4L2BUF_t *                        mCurrentV4l2buf;

    bool                            mVideoHint;

    VideoDecoder *                    mDecoder;
    VConfig                            mVideoConf;
    VideoStreamInfo                    mVideoInfo;
    VideoStreamDataInfo                mDataInfo;
    /*support of libve*/
/*    ve_mutex_t mDecoderMutex;
    cedarv_decoder_t * mDecoder;
    cedarv_stream_info_t        mStream_info;
    cedarv_stream_data_info_t   mData_info;
*/

#define STATISICS_CNT    60
    int                                mCurAvailBufferCnt;
    int                                mStatisicsIndex;
    bool                            mNeedHalfFrameRate;
    bool                            mShouldPreview;

    bool                            mIsThumbUsedForVideo;
    int                                mVideoWidth;            // for cts
    int                                mVideoHeight;
    int                             mDiscardFrameNum;

    struct ScCamMemOpsS* mMemOpsS;
};

}; /* namespace android */

#endif  /* __HAL_V4L2_CAMERA_DEVICE_H__ */
