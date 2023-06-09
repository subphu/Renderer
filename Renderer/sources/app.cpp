#include "app.h"

#include <system.h>
#include <file.h>
#include <structure/2DStructure/structure2D.h>
#include <structure/defaultStructure/defaultStructure.h>

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

	mStructurePtr = new Structure2D();

	mWatcher = new Watcher();
	mWatcher->watchShaderDir();

	System::setDeviceManager( mDeviceManagerPtr );
	System::setWindow		( mSdlWindowPtr );
	System::setCmdManager	( mCmdManagerPtr );
	System::setSwapchain	( mSwapchainPtr );
	System::setDescriptor	( mDescriptorPtr );
	System::setWatcher		( mWatcher );

	File::CreateDir( mSettingsPtr->spirvDir );
}

void App::setup() {
	mCmdManagerPtr->createPool();
	mDescriptorPtr->createPool();
	mSwapchainPtr->create( mSdlWindowPtr->getSurface() );
	mSwapchainPtr->createFrames();

	mStructurePtr->setup();

	mDescriptorPtr->allocateDescriptorSets();
}

void App::loop() {
	RenderTime renderTime{};
	bool quit = false;
	bool lockFps = false;

	mSdlWindowPtr->updateInput();
	while (!mSdlWindowPtr->isClosed()) {
		mSdlWindowPtr->pollEvent();
		mSdlWindowPtr->updateInput();

		mStructurePtr->update( renderTime );
		
		renderTime.subPresentTime();
		while (renderTime.isLag()) {
		
			mStructurePtr->draw();
			
			renderTime.addRenderTime();
			renderTime.sleepIf( lockFps );
		}
	}
}

void App::cleanup() {
	mDeviceManagerPtr->getDevice()->waitQueueIdle();
	mDeviceManagerPtr->getDevice()->waitDeviceIdle();

	mWatcher->cleanup();

	mStructurePtr->cleanup();

	mSwapchainPtr->cleanup();
	mDescriptorPtr->cleanup();
	mCmdManagerPtr->cleanup();
}

void App::end() {
	mSdlWindowPtr->cleanup();
	mDeviceManagerPtr->cleanup();
}
