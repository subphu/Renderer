#pragma once

#include <settings.h>
#include <window/sdlwindow.h>
#include <core/deviceManager.h>
#include <core/cmdManager.h>
#include <core/swapchain.h>

class App {

public:

	void start();

private:

	void init();
	void setup();
	void loop();
	void cleanup();
	void end();
	
	Settings*		mSettingsPtr	  = nullptr;
	SDLWindow*		mSdlWindowPtr	  = nullptr;
	DeviceManager*	mDeviceManagerPtr = nullptr;
	CmdManager*		mCmdManagerPtr	  = nullptr;
	Swapchain*		mSwapchain		  = nullptr;

};

