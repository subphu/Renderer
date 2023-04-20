#pragma once

#include <common.h>
#include <settings.h>
#include <watcher.h>
#include <window/sdlwindow.h>
#include <core/deviceManager.h>
#include <core/cmdManager.h>
#include <core/swapchain.h>
#include <core/descriptor.h>

struct RenderTime {
	TimeVal lastTime = ChronoTime::now();
	f32 delay = 1.f / 60.f;
	f32 lag = delay;
	f32 framerate = 0;
	u32 frameNum = 0;
	u32 millisecond = 0;
	u32 microsecond = 0;
	bool lock = false;

	bool isLag() { return lag < delay; }
	u32 getSleepTime() { return U32((delay - lag) * 1000000); }

	void updateTime() { 
		const u32 ns = 1000000000;
		const u32 ms = 1000000;
		lastTime = ChronoTime::now();
		auto epoch = lastTime.time_since_epoch();
		u32 prevMicro = microsecond;
		millisecond = U32( std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count() % ns );
		microsecond = U32( std::chrono::duration_cast<std::chrono::microseconds>(epoch).count() % ns );
		framerate = f32(ms) / (microsecond - prevMicro);
		frameNum++;
	};

	void subPresentTime() { lag -= delay; };
	void addRenderTime() {
		lag += TimeDif( ChronoTime::now() - lastTime ).count();
		updateTime();
	};

	void sleepIf( bool condition ) {
		if (!condition || lag > delay) return;
		Sleep( getSleepTime() );
		addRenderTime();
	};
};

class System {
private:
	~System() {}
	System() {}

	Settings*		mSettingsPtr		= new struct Settings();
	SDLWindow*		mWindowPtr			= nullptr;
	DeviceManager*	mDeviceManagerPtr	= nullptr;
	CmdManager*		mCmdManagerPtr		= nullptr;
	Swapchain*		mSwapchainPtr		= nullptr;
	Descriptor*		mDescriptorPtr		= nullptr;
	Watcher*		mWatcher			= nullptr;

public:

	static auto& Instance() { static System instance; return instance; }

	static Settings*	Settings()	 { return Instance().mSettingsPtr; }
	static SDLWindow*	Window()	 { return Instance().mWindowPtr; }
	static Device*		Device()	 { return Instance().mDeviceManagerPtr->getDevice(); }
	static CmdManager*	CmdManager() { return Instance().mCmdManagerPtr; }
	static Swapchain*	Swapchain()  { return Instance().mSwapchainPtr; }
	static Descriptor*	Descriptor() { return Instance().mDescriptorPtr; }
	static Watcher*		Watcher()	 { return Instance().mWatcher; }

	static void setSettings		( struct Settings* settings )			{ Instance().mSettingsPtr = settings; }
	static void setWindow		( class SDLWindow* window )				{ Instance().mWindowPtr = window; }
	static void setDeviceManager( class DeviceManager* deviceManager )	{ Instance().mDeviceManagerPtr = deviceManager; }
	static void setCmdManager	( class CmdManager* cmdManagerPtr )		{ Instance().mCmdManagerPtr = cmdManagerPtr; }
	static void setSwapchain	( class Swapchain* swapchainPtr )		{ Instance().mSwapchainPtr = swapchainPtr; }
	static void setDescriptor	( class Descriptor* descriptor )		{ Instance().mDescriptorPtr = descriptor; }
	static void setWatcher		( class Watcher* watcher )				{ Instance().mWatcher = watcher; }
	
	System( const System& ) = delete;
	System& operator=( const System& ) = delete;
	System( System&& ) = delete;
	System& operator=( System&& ) = delete;
};
