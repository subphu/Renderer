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

	void waitIdle();
	void waitPresentQueueIdle();
	
	VkDevice		 getDevice();
	VkPhysicalDevice getPhysicalDevice();

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

