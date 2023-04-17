#pragma once

#include <common.h>

struct SDL_Window;

class SDLWindow {

public:
	~SDLWindow();
	SDLWindow( const UInt2& size, const c8* name );

	void cleanup();

	void updateInstanceExtension();
	void createSurface( const VkInstance instance );

	void pollEvent();

	u32 getWindowId();
	f32 getRatio();
	UInt2 getSize();
	UInt2 getFrameSize();

	void setSize( const UInt2& size );
	bool isResized();
	bool isClosed();

	const VkSurfaceKHR& getSurface();

	void updateInput();
	const vec4 getMousePos();
	const vec4 getMouseDownDelta();
	const uvec2 getMouseState();

private:
	Cleaner mCleaner{};

	SDL_Window* mWindowPtr { nullptr };

	VkSurfaceKHR mSurface = VK_NULL_HANDLE;

	bool mResized = false;
	bool mClosed = false;

	vec4 mMousePos;
	vec4 mMouseDownDelta;
	uvec2 mMouseState;
};

