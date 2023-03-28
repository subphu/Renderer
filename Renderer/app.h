#pragma once

#include <settings.h>
#include <window/sdlwindow.h>
#include <core/deviceManager.h>

class App {

public:

	void run();

private:

	void init();
	void setup();
	void loop();
	void cleanup();

	SDLWindow*		mSdlWindowPtr	  = nullptr;
	DeviceManager*	mDeviceManagerPtr = nullptr;
	Settings*		mSettingsPtr	  = nullptr;

};

