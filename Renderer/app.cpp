#include "app.h"

#include <system.h>

void App::start() {
	init();
	setup();
	loop();
	cleanup();
	end();
}

void App::init() {
	mSettingsPtr = new struct Settings();

	mDeviceManagerPtr = new DeviceManager();
	mSdlWindowPtr = new SDLWindow( mSettingsPtr->windowSize, mSettingsPtr->appName );

	mSdlWindowPtr->updateInstanceExtension();
	mDeviceManagerPtr->createInstance();

	mSdlWindowPtr->createSurface( mDeviceManagerPtr->getInstance() );
	mDeviceManagerPtr->createDevices( mSdlWindowPtr->getSurface() );

	mCmdManagerPtr = new CmdManager();

	System::setDeviceManager( mDeviceManagerPtr );
	System::setWindow		( mSdlWindowPtr );
	System::setCmdManager	( mCmdManagerPtr );
}

void App::setup() {
	mCmdManagerPtr->createPool();
}

void App::loop() {
	RenderTime renderTime{};
	bool quit = false;
	bool lockFps = false;

	while (!mSdlWindowPtr->isClosed()) {
		mSdlWindowPtr->pollEvent();
		//update();

		renderTime.startRender();
		while (renderTime.isLag()) {
			//draw();
			renderTime.addRenderTime();
			renderTime.sleepIf( lockFps );
		}
	}
}

void App::cleanup() {
	mCmdManagerPtr->cleanup();
}

void App::end() {
	mSdlWindowPtr->cleanup();
	mDeviceManagerPtr->cleanup();
}
