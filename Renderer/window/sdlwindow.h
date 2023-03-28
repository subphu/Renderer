#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#undef main

#include <common.h>

class SDLWindow {

public:
	~SDLWindow();
	SDLWindow( const UInt2& size, const char* name );

	void close();

	void createSurface( const VkInstance& instance );

	void pollEvent();

	u32 getWindowId();
	f32 getRatio();
	UInt2 getSize();
	UInt2 getFrameSize();

	void setSize( const UInt2& size );
	bool isResized();
	bool isClosed();

	const VkSurfaceKHR& getSurface();

private:
	SDL_Window* mWindowPtr { nullptr };

	VkSurfaceKHR mSurface = VK_NULL_HANDLE;
	VkInstance mInstance = VK_NULL_HANDLE;

	bool mResized = false;
	bool mClosed = false;

	void getInstanceExtension();
};

