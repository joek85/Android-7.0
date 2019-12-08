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

#ifndef __SMILE_DETECTION_API_H___
#define __SMILE_DETECTION_API_H___

#include <utils/StrongPointer.h>

namespace android {

/*input and output data*/
typedef struct FacePosition
{
    int faceTopLeftX;
    int faceTopLeftY;
    
    int faceWidth;
    int faceHeigth;
}FacePosition;

typedef struct FrameFaceData
{
    FacePosition *facePositions;
    int faceNum;
    
    unsigned char *frameData;
    int frameWidth;
    int frameHeight;
    
    int angle;//0,90,180,270
}FrameFaceData;

typedef struct Status
{
    int num;
    int *sta;
}Statuss;

/*Smile Detect*/
typedef int (*smile_notify_cb)(int cmd, void * data, void *user);

enum SMILE_NOTITY_CMD{
    SMILE_NOTITY_CMD_RESULT,
};

class SmileDetection;

enum SMILE_OPS_CMD
{
    SMILE_OPS_CMD_START,
    SMILE_OPS_CMD_STOP,
    SMILE_OPS_CMD_REGISTE_USER,
};

struct SmileDetectionDev
{
    void * user;
    sp<SmileDetection> priv;
    void (*setCallback)(SmileDetectionDev * dev, smile_notify_cb cb);
    int (*ioctrl)(SmileDetectionDev * dev, int cmd, int para0, FrameFaceData *para1);
};

extern int CreateSmileDetectionDev(SmileDetectionDev ** dev);
extern void DestroySmileDetectionDev(SmileDetectionDev * dev);

/*eye blink detection*/
typedef int (*eye_blink_notify_cb)(int cmd, void * data, void *user);

enum EYE_BLINK_NOTITY_CMD{
    EYE_BLINK_NOTITY_CMD_RESULT,
};

class EyeBlinkDetection;

enum EYE_BLINK_OPS_CMD
{
    EYE_BLINK_OPS_CMD_START,
    EYE_BLINK_OPS_CMD_STOP,
    EYE_BLINK_OPS_CMD_REGISTE_USER,
};

struct EyeBlinkDetectionDev
{
    void * user;
    sp<EyeBlinkDetection> priv;
    void (*setCallback)(EyeBlinkDetectionDev * dev, eye_blink_notify_cb cb);
    int (*ioctrl)(EyeBlinkDetectionDev * dev, int cmd, int para0, FrameFaceData *para1);
};

extern int CreateEyeBlinkDetectionDev(EyeBlinkDetectionDev ** dev);
extern void DestroyEyeBlinkDetectionDev(EyeBlinkDetectionDev * dev);

}

#endif    // __FACE_DETECTION_API_H__
