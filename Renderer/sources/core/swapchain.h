//  Copyright © 2021 Subph. All rights reserved.
//

#pragma once

#include <common.h>

class Image;

class Swapchain {
	
public:
	~Swapchain();
	Swapchain();
	
	void cleanup();
	
	void create( const VkSurfaceKHR surface );
	void createFrames();

	bool prepare( VkCommandBuffer* cmdBuffer );
	bool present( Image* image );

	const VkCommandBuffer getCmdBuffer();
	
private:
	Cleaner mCleaner{};

	VkSwapchainCreateInfoKHR mSwapchainInfo{};
	VkSwapchainKHR mSwapchain;

	u32 mTotalFrame = 0;
	u32 mImageIdx = 0;
	u32 mSemaphoreIdx = 0;
	
	vector<VkImage> mImages;
	vector<VkFence> mSubmitFences;
	vector<VkSemaphore> mSubmitSemaphores;
	vector<VkSemaphore> mPrepareSemaphores;
	vector<VkCommandBuffer> mCmdBuffers;

	void fillFrame( Image* image );
	void submitFrame();
	bool presentFrame();

	bool checkSwapchainResult(VkResult result);
};
