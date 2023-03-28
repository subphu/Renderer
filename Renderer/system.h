#pragma once

#include <common.h>
#include <settings.h>
#include <window/sdlwindow.h>
#include <core/deviceManager.h>
#include <core/cmdManager.h>

struct RenderTime {
	TimeVal lastTime = ChronoTime::now();
	f32 delay = 1.f / 60.f;
	f32 lag = delay;
	bool lock = false;

	bool isLag() { return lag < delay; }
	u32 getSleepTime() { return U32((delay - lag) * 1000000); }

	void updateTime() { lastTime = ChronoTime::now(); };
	void startRender() { lag -= delay; };
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

	Settings* mSettingsPtr = new struct Settings();
	SDLWindow* mWindowPtr = nullptr;
	DeviceManager* mDeviceManagerPtr = nullptr;
	CmdManager* mCmdManagerPtr = nullptr;

public:

	static auto& Instance() { static System instance; return instance; }

	static Settings*	Settings()	 { return Instance().mSettingsPtr; }
	static SDLWindow*	Window()	 { return Instance().mWindowPtr; }
	static Device*		Device()	 { return Instance().mDeviceManagerPtr->getDevice(); }
	static CmdManager*	CmdManager() { return Instance().mCmdManagerPtr; }

	static void setSettings		( struct Settings* settings )			{ Instance().mSettingsPtr = settings; }
	static void setWindow		( class SDLWindow* window )				{ Instance().mWindowPtr = window; }
	static void setDeviceManager( class DeviceManager* deviceManager )	{ Instance().mDeviceManagerPtr = deviceManager; }
	static void setCmdManager	( class CmdManager* cmdManagerPtr )		{ Instance().mCmdManagerPtr = cmdManagerPtr; }
	
	System( const System& ) = delete;
	System& operator=( const System& ) = delete;
	System( System&& ) = delete;
	System& operator=( System&& ) = delete;
};
