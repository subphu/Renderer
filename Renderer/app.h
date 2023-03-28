#pragma once

#include <settings.h>
#include <window/sdlwindow.h>
#include <core/deviceManager.h>
#include <core/cmdManager.h>

class App {

public:

	void start();

private:

	void init();
	void setup();
	void loop();
	void cleanup();
	void end();

	SDLWindow*		mSdlWindowPtr	  = nullptr;
	DeviceManager*	mDeviceManagerPtr = nullptr;
	CmdManager*		mCmdManagerPtr	 = nullptr;
	Settings*		mSettingsPtr	  = nullptr;

};

