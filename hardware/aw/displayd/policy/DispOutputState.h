/*
* Copyright (C) 2008 The Android Open Source Project
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

#ifndef _DISPOUTPUTSTATE_H_
#define _DISPOUTPUTSTATE_H_

class DisplayPolicy;

class DispOutputState {

public:
	DisplayPolicy *mDisplayPolicy;

	DispOutputState();
	virtual ~DispOutputState() {}
	virtual void devicePlugChanged(int /*type*/, int /*priority*/, bool /*isPlugIn*/){}
};

class MainDispToDev0PlugIn : public DispOutputState {
public:
	MainDispToDev0PlugIn(DisplayPolicy *dp);
	virtual ~MainDispToDev0PlugIn() {}
	void devicePlugChanged(int type,int priority,bool isPlugIn);
};

class MainDispToDev0PlugInExt : public DispOutputState {
public:
	MainDispToDev0PlugInExt(DisplayPolicy *dp);
	virtual ~MainDispToDev0PlugInExt() {}
	void devicePlugChanged(int type,int priority,bool isPlugIn);
};

class MainDispToDev0PlugOut : public DispOutputState {
public:
	MainDispToDev0PlugOut(DisplayPolicy *dp);
	virtual ~MainDispToDev0PlugOut() {}
	void devicePlugChanged(int type,int priority,bool isPlugIn);
};

class MainDispToDev1PlugIn : public DispOutputState {
public:
	MainDispToDev1PlugIn(DisplayPolicy *dp);
	virtual ~MainDispToDev1PlugIn() {}
	void devicePlugChanged(int type,int priority,bool isPlugIn);
};

class MainDispToDev1PlugOut : public DispOutputState {
public:
	MainDispToDev1PlugOut(DisplayPolicy *dp);
	virtual ~MainDispToDev1PlugOut() {}
	void devicePlugChanged(int type,int priority,bool isPlugIn);
};

class DualDisplay : public DispOutputState {
public:
	DualDisplay(DisplayPolicy *dp);
	virtual ~DualDisplay() {}
	void devicePlugChanged(int type,int priority,bool isPlugIn);
};

#endif //ifndef _DISPOUTPUTSTATE_H_

