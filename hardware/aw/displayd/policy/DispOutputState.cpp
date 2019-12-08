/*
* Copyright (C) 2011 The Android Open Source Project
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

#include "DispOutputState.h"
#include "DisplayPolicy.h"
#include <cutils/log.h>

DispOutputState::DispOutputState() {}

MainDispToDev0PlugIn::MainDispToDev0PlugIn(DisplayPolicy *dp)
{
	mDisplayPolicy = dp;
}

void MainDispToDev0PlugIn::devicePlugChanged(int type,int priority,bool isPlugIn)
{
    if(!isPlugIn && (0 == priority)) {
        mDisplayPolicy->setOutputState(mDisplayPolicy->getMainDispToDev0PlugOut());
    } else if((1 == priority) && isPlugIn) {
        if(1 == mDisplayPolicy->getPolicy()) {
            mDisplayPolicy->setOutputState(mDisplayPolicy->getMainDispToDev0PlugInExt());
        } else {
            if(0 == mDisplayPolicy->setDisplayOutput(1, type, 0xFF)) {
                mDisplayPolicy->setOutputState(mDisplayPolicy->getDualDisplayOutput());
            } else {
                mDisplayPolicy->setOutputState(mDisplayPolicy->getMainDispToDev0PlugInExt());
            }
        }
    }
	return;
}

MainDispToDev0PlugInExt::MainDispToDev0PlugInExt(DisplayPolicy *dp)
{
	mDisplayPolicy = dp;
}

void MainDispToDev0PlugInExt::devicePlugChanged(int type,int priority,bool isPlugIn)
{
    if(!isPlugIn) {
        if(0 == priority) {
            if(0 == mDisplayPolicy->setDisplayOutput(0, type, 0xFF)) {
                mDisplayPolicy->setOutputState(mDisplayPolicy->getMainDispToDev1PlugIn());
            } else {
                // the device with d1Format will not to display on mainDisp since it display on other way.
                mDisplayPolicy->setOutputState(mDisplayPolicy->getMainDispToDev0PlugOut());
            }
        } else if(1 == priority) {
            mDisplayPolicy->setDisplayOutput(1, 0, 0);
            mDisplayPolicy->setOutputState(mDisplayPolicy->getMainDispToDev0PlugIn());
        }
    }
	return;
}

MainDispToDev0PlugOut::MainDispToDev0PlugOut(DisplayPolicy *dp)
{
	mDisplayPolicy = dp;
}

void MainDispToDev0PlugOut::devicePlugChanged(int type,int priority,bool isPlugIn)
{
    if(isPlugIn) {
        if(0 == priority) {
            mDisplayPolicy->setDisplayOutput(0, type, 0xFF);
            mDisplayPolicy->setOutputState(mDisplayPolicy->getMainDispToDev0PlugIn());
        } else if(1 == priority) {
            if(0 == mDisplayPolicy->setDisplayOutput(0, type, 0xFF)) {
                mDisplayPolicy->setOutputState(mDisplayPolicy->getMainDispToDev1PlugIn());
            } else {
                // the device with this format maybe failed to display on mainDisp. it displays on other way.
                mDisplayPolicy->setOutputState(mDisplayPolicy->getMainDispToDev0PlugInExt());
            }
        }
    }
	return;
}

MainDispToDev1PlugIn::MainDispToDev1PlugIn(DisplayPolicy *dp)
{
	mDisplayPolicy = dp;
}

void MainDispToDev1PlugIn::devicePlugChanged(int type,int priority,bool isPlugIn)
{
    if(!isPlugIn && (1 == priority)) {
        mDisplayPolicy->setOutputState(mDisplayPolicy->getMainDispToDev1PlugOut());
    } else if(isPlugIn && (0 == priority)) {
        // hwc will exchange on calling mDMP.setDisplayOutput(DISPLAY_EXTERNAL, d0Format),
        // than the device with this format will be on mainDisp.
        // if it return success(0), mean DualDisplayOutput,
        // otherwise only mainDisp exist, the externalDisp was be closed or displays on other way.
        if(1 == mDisplayPolicy->getPolicy()) {
           mDisplayPolicy->setDisplayOutput(0, type, 0xFF);
           mDisplayPolicy->setOutputState(mDisplayPolicy->getMainDispToDev0PlugInExt());
        } else {
            if(0 == mDisplayPolicy->setDisplayOutput(1, type, 0xFF)) {
                mDisplayPolicy->setOutputState(mDisplayPolicy->getDualDisplayOutput());
            } else {
                mDisplayPolicy->setOutputState(mDisplayPolicy->getMainDispToDev0PlugInExt());
            }
        }
    }
	return;
}

MainDispToDev1PlugOut::MainDispToDev1PlugOut(DisplayPolicy *dp)
{
	mDisplayPolicy = dp;
}

void MainDispToDev1PlugOut::devicePlugChanged(int type,int priority,bool isPlugIn)
{
    if(isPlugIn) {
        if(0 == priority) {
            mDisplayPolicy->setDisplayOutput(0, type, 0xFF);
            mDisplayPolicy->setOutputState(mDisplayPolicy->getMainDispToDev0PlugIn());
        } else if(1 == priority) {
            mDisplayPolicy->setOutputState(mDisplayPolicy->getMainDispToDev1PlugIn());
        }
    }
	return;
}

DualDisplay::DualDisplay(DisplayPolicy *dp)
{
	mDisplayPolicy = dp;
}

void DualDisplay::devicePlugChanged(int type,int priority,bool isPlugIn)
{
    if(!isPlugIn) {
        if(0 == priority) {
            mDisplayPolicy->setDisplayOutput(0, type, 0xFF);
            mDisplayPolicy->setDisplayOutput(1, 0, 0);
            mDisplayPolicy->setOutputState(mDisplayPolicy->getMainDispToDev1PlugIn());
        } else if(1 == priority) {
            mDisplayPolicy->setDisplayOutput(1, 0, 0);
            mDisplayPolicy->setOutputState(mDisplayPolicy->getMainDispToDev0PlugIn());
        }
    }
	return;
}

