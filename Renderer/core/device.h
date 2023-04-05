//  Copyright © 2021 Subph. All rights reserved.
//

#pragma once

#include <set>

#include <common.h>

class Device {

public:

	~Device();
	Device( const VkPhysicalDevice physicalDevice );
	
	void cleanup();
	
	bool isSupported( const VkSurfaceKHR surface );
	void createDevice();

	void usePresentMode( const VkPresentModeKHR mode );
	void useSurfaceFormat( const VkSurfaceFormatKHR surfaceFormat );

	void waitDeviceIdle();
	void waitQueueIdle();

	u32 findMemoryTypeIndex( const u32 typeFilter, const VkMemoryPropertyFlags flags );
	VkFormatProperties getFormatProperties( const VkFormat format );

	VkDevice		 getDevice();
	VkPhysicalDevice getPhysicalDevice();

	VkSurfaceCapabilitiesKHR getSurfaceCapabilities();
	VkSurfaceFormatKHR getSurfaceFormat();
	VkPresentModeKHR   getPresentMode();

	VkQueue	getGraphicQueue();
	VkQueue	getPresentQueue();
	u32		getGraphicQueueIndex();
	u32		getPresentQueueIndex();

private:
	Cleaner mCleaner{};

	VkDevice mDevice;
	VkPhysicalDevice mPhysicalDevice;

	VkPhysicalDeviceProperties mProperties;
	VkPhysicalDeviceMemoryProperties mMemoryProperties;
	VkSurfaceCapabilitiesKHR mSurfaceCapabilities;

	s32 mPresentQueueIndex;
	s32 mGraphicQueueIndex;
	VkQueue mGraphicQueue;
	VkQueue mPresentQueue;

	bool mSupported;

	vector<VkSurfaceFormatKHR> mSurfaceFormats;
	vector<VkPresentModeKHR>   mPresentModes;
	VkSurfaceFormatKHR mSurfaceFormat{};
	VkPresentModeKHR   mPresentMode  {};

};

