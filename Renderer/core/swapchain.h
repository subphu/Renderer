//  Copyright © 2021 Subph. All rights reserved.
//

#pragma once

#include <common.h>

class RpPresent;

class Swapchain {
	
public:
	~Swapchain();
	Swapchain();
	
	void cleanup();
	
	void create( const VkSurfaceKHR surface );
	void createFrames();
	
	void prepareFrame();
	void submitFrame();
	void presentFrame();

	RpPresent* getRenderpass();
	
private:
	Cleaner mCleaner{};

	VkSwapchainCreateInfoKHR mSwapchainInfo{};
	VkSwapchainKHR mSwapchain;

	RpPresent* mRenderpass;

	u32 mTotalFrame = 0;
	u32 mActiveIdx = 0;
	u32 mSemaphoreIdx = 0;
	
	vector<VkFence> mSubmitFences;
	vector<VkSemaphore> mSubmitSemaphores;
	vector<VkSemaphore> mPrepareSemaphores;
	vector<VkCommandBuffer> mCommandBuffers;
	
	void checkSwapchainResult(VkResult result);
};
