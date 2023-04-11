#include "app.h"

#include <system.h>
#include <structure/defaultStructure.h>

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
	mSwapchainPtr = new Swapchain();
	mDescriptorPtr = new Descriptor();

	mStructurePtr = new DefaultStructure();

	System::setDeviceManager( mDeviceManagerPtr );
	System::setWindow		( mSdlWindowPtr );
	System::setCmdManager	( mCmdManagerPtr );
	System::setSwapchain	( mSwapchainPtr );
	System::setDescriptor	( mDescriptorPtr );

}

void App::setup() {
	mCmdManagerPtr->createPool();
	mDescriptorPtr->createPool();
	mSwapchainPtr->create( mSdlWindowPtr->getSurface() );
	mSwapchainPtr->createFrames();

	mStructurePtr->setup();

}

void App::loop() {
	RenderTime renderTime{};
	bool quit = false;
	bool lockFps = false;

	while (!mSdlWindowPtr->isClosed()) {
		mSdlWindowPtr->pollEvent();
		mStructurePtr->update();
		renderTime.startRender();
		while (renderTime.isLag()) {
			mStructurePtr->draw();
			renderTime.addRenderTime();
			renderTime.sleepIf( lockFps );
		}
	}
}

void App::cleanup() {
	mDeviceManagerPtr->getDevice()->waitDeviceIdle();

	mStructurePtr->cleanup();

	mSwapchainPtr->cleanup();
	mDescriptorPtr->cleanup();
	mCmdManagerPtr->cleanup();
}

void App::end() {
	mSdlWindowPtr->cleanup();
	mDeviceManagerPtr->cleanup();
}
