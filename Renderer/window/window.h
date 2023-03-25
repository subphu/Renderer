#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <include.h>

class Window {

public:
    ~Window();
	Window( const UInt2& size, const char* name );

	void close();

    void getInstanceExtension( vector<const char*>& extensions );
    const VkSurfaceKHR& createSurface( const VkInstance& instance );

    u32 getWindowId();
    f32 getRatio();
    UInt2 getSize();
    UInt2 getFrameSize();

    void setSize( const UInt2& size );
    void notifyResize();
    bool isResized();


private:
	SDL_Window* mWindow{ nullptr };

    bool mResized;
    VkSurfaceKHR mSurface;

};

