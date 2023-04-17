#include "sdlwindow.h"
#include <system.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#undef main

SDLWindow::~SDLWindow() {}

SDLWindow::SDLWindow( const UInt2& size, const c8* name ) {
	SDL_Init( SDL_INIT_VIDEO );
	mWindowPtr = SDL_CreateWindow(
		name,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		size.width,
		size.height,
		SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MOUSE_CAPTURE
	);
	ASSERT_NULLPTR( mWindowPtr, "Failed to create SDL window" );
	mCleaner.push( [=]() { SDL_DestroyWindow( mWindowPtr ); } );
}

void SDLWindow::cleanup() { mCleaner.flush( "SDLWindow" ); }

void SDLWindow::updateInstanceExtension() {
	vector<const c8*>& extensions = System::Settings()->instanceExtensions;
	u32 count, prevCount = U32(extensions.size());
	SDL_Vulkan_GetInstanceExtensions( mWindowPtr, &count, nullptr );
	extensions.resize( SIZE_T( count ) + SIZE_T( prevCount ) );
	SDL_Vulkan_GetInstanceExtensions( mWindowPtr, &count, extensions.data() + prevCount );
}

void SDLWindow::createSurface( const VkInstance instance ) {
	SDL_Vulkan_CreateSurface( mWindowPtr, instance, &mSurface );
	mCleaner.push( [=]() { vkDestroySurfaceKHR( instance, mSurface, nullptr ); } );
}

void SDLWindow::pollEvent() {
	SDL_Event e{};
	while (SDL_PollEvent( &e ) != 0) {
		if (e.type == SDL_QUIT) {
			SDL_Log( "Window quit" );
			mClosed = true;
		}
		if (e.type == SDL_WINDOWEVENT) {
			switch (e.window.event) {
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				SDL_Log( "Window %d size changed to %dx%d",
					e.window.windowID, e.window.data1,
					e.window.data2 );
				break;
			default:
				SDL_Log( "Window %d got unknown e %d",
					e.window.windowID, e.window.event );
				break;
			}
		}
	}
}

u32 SDLWindow::getWindowId() {
	return SDL_GetWindowID( mWindowPtr );
}

f32 SDLWindow::getRatio() {
	UInt2 size = getFrameSize();
	return F32( size.width ) / F32( size.height );
}

UInt2 SDLWindow::getSize() {
	s32 width, height;
	SDL_GetWindowSize( mWindowPtr, &width, &height );
	UInt2 size{ U32( width ), U32( height ) };
	return size;
}

UInt2 SDLWindow::getFrameSize() {
	s32 width, height;
	SDL_Vulkan_GetDrawableSize( mWindowPtr, &width, &height );
	UInt2 size{ U32( width ), U32( height ) };
	return size;
}

void SDLWindow::setSize( const UInt2& size ) {
	SDL_SetWindowSize( mWindowPtr, size.width, size.height );
}

bool SDLWindow::isResized() { return mResized; }
bool SDLWindow::isClosed() { return mClosed; }
const VkSurfaceKHR& SDLWindow::getSurface() { return mSurface; }

void SDLWindow::updateInput() {
	s32 x, y;
	mMouseState.y = mMouseState.x;
	mMouseState.x = SDL_GetMouseState( &x, &y );
	mMousePos.z = f32( x ) - mMousePos.x;
	mMousePos.w = f32( y ) - mMousePos.y;
	mMousePos.x = f32( x );
	mMousePos.y = f32( y );
	if (mMouseState.x == SDL_BUTTON( SDL_BUTTON_LEFT )) {
		mMouseDownDelta.x += mMousePos.z;
		mMouseDownDelta.y += mMousePos.w;
	}
	if (mMouseState.x == SDL_BUTTON( SDL_BUTTON_RIGHT )) {
		mMouseDownDelta.z += mMousePos.z;
		mMouseDownDelta.w += mMousePos.w;
	}
}

const vec4 SDLWindow::getMousePos() { return mMousePos; }

const vec4 SDLWindow::getMouseDownDelta() { return mMouseDownDelta; }

const uvec2 SDLWindow::getMouseState() { return mMouseState; }
