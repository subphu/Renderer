#include "app.h"

#include <system.h>

void App::run() {
	init();
	setup();
	loop();
	cleanup();
}

void App::init() {
	mSettingsPtr = new struct Settings();

	mSdlWindowPtr = new SDLWindow( mSettingsPtr->windowSize, mSettingsPtr->appName );
	mDeviceManagerPtr = new DeviceManager();
	mDeviceManagerPtr->createInstance();

	mSdlWindowPtr->createSurface( mDeviceManagerPtr->getInstance() );
	mDeviceManagerPtr->createDevices( mSdlWindowPtr->getSurface() );

}

void App::setup() {}

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

void App::cleanup() {}
