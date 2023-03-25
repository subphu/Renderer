#include "window.h"

namespace {
	static int resizeEvent( void* data, SDL_Event* event ) {
		if (event->type != SDL_WINDOWEVENT || event->window.event != SDL_WINDOWEVENT_RESIZED)
			return 0;

		Window* window = reinterpret_cast<Window*>(data);
		if (event->window.windowID == window->getWindowId()) {
			window->notifyResize();
		}
		return 1;
	}
}

Window::~Window() {}

Window::Window( const UInt2& size, const char* name ) :
	mResized( false ),
	mSurface( VK_NULL_HANDLE )
{
	SDL_Init( SDL_INIT_VIDEO );
	mWindow = SDL_CreateWindow(
		name,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		size.width,
		size.height,
		SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MOUSE_CAPTURE
	);
	ERR_IF_NULLPTR( mWindow, "Failed to create SDL window" );

	SDL_AddEventWatch( resizeEvent, this );
}

void Window::close() {
	SDL_DestroyWindow( mWindow );
}

void Window::getInstanceExtension( vector<const char*>& extensions ) {
	u32 count, prevCount = extensions.size();
	SDL_Vulkan_GetInstanceExtensions( mWindow, &count, nullptr );
	extensions.resize( count + prevCount );
	SDL_Vulkan_GetInstanceExtensions( mWindow, &count, extensions.data() + prevCount );
}

const VkSurfaceKHR& Window::createSurface( const VkInstance& instance ) {
	SDL_Vulkan_CreateSurface( mWindow, instance, &mSurface );
	return mSurface;
}

u32 Window::getWindowId() {
	return SDL_GetWindowID( mWindow );
}

f32 Window::getRatio() {
	UInt2 size = getFrameSize();
	return F32( size.width ) / F32( size.height );
}

UInt2 Window::getSize() {
	s32 width, height;
	SDL_GetWindowSize( mWindow, &width, &height );
	return UInt2( width, height );
}

UInt2 Window::getFrameSize() {
	s32 width, height;
	SDL_Vulkan_GetDrawableSize( mWindow, &width, &height );
	return UInt2( width, height );
}

void Window::setSize( const UInt2& size ) {
	SDL_SetWindowSize( mWindow, size.width, size.height );
}

void Window::notifyResize() {
	mResized = true;
}

bool Window::isResized() {
	bool result = mResized;
	mResized = false;
	return result;
}
