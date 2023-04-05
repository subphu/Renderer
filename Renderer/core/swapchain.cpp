//  Copyright © 2021 Subph. All rights reserved.
//

#include "swapchain.h"
#include <system.h>
#include <renderpass/rpPresent.h>

namespace {
	vector<VkImage> GetSwapchainImages( VkSwapchainKHR swapchain ) {
		VkDevice device = System::Device()->getDevice();
		u32 count;
		vkGetSwapchainImagesKHR( device, swapchain, &count, nullptr );
		vector<VkImage> swapchainImages( count );
		vkGetSwapchainImagesKHR( device, swapchain, &count, swapchainImages.data() );
		return swapchainImages;
	}
}

Swapchain::~Swapchain() {}
Swapchain::Swapchain() {}

void Swapchain::cleanup() { mCleaner.flush("Swapchain"); }

void Swapchain::create( const VkSurfaceKHR surface ) {
	LOG( "Swapchain::create" );
	Device*		   devicePtr = System::Device();
	const VkDevice device    = devicePtr->getDevice();
	const VkPresentModeKHR         presentMode   = devicePtr->getPresentMode();
	const VkSurfaceFormatKHR       surfaceFormat = devicePtr->getSurfaceFormat();
	const VkSurfaceCapabilitiesKHR capabilities  = devicePtr->getSurfaceCapabilities();
	
	mSwapchainInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	mSwapchainInfo.surface          = surface;
	mSwapchainInfo.preTransform     = capabilities.currentTransform;
	mSwapchainInfo.minImageCount    = capabilities.maxImageCount < 3 ? 2 : 3;
	mSwapchainInfo.imageExtent      = capabilities.currentExtent;
	mSwapchainInfo.imageFormat      = surfaceFormat.format;
	mSwapchainInfo.imageColorSpace  = surfaceFormat.colorSpace;
	mSwapchainInfo.imageArrayLayers = 1;
	mSwapchainInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	mSwapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	mSwapchainInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	mSwapchainInfo.presentMode      = presentMode;
	mSwapchainInfo.clipped          = VK_TRUE;
	mSwapchainInfo.oldSwapchain     = VK_NULL_HANDLE;

	VkResult result = vkCreateSwapchainKHR( devicePtr->getDevice(), &mSwapchainInfo, nullptr, &mSwapchain );
	ASSERT_VKERROR(result, "failed to create swapchain!");
	mCleaner.push([=](){ vkDestroySwapchainKHR(device, mSwapchain, nullptr); });
}

void Swapchain::createFrames() {
	LOG("Swapchain::createFrames");
	VkDevice device = System::Device()->getDevice();
	CmdManager* cmdManager = System::CmdManager();

	vector<VkImage> swapchainImages = GetSwapchainImages(mSwapchain);
	mTotalFrame = U32(swapchainImages.size());
	mRenderpass = new RpPresent();
	mRenderpass->create();
	mRenderpass->createFrames( swapchainImages, mSwapchainInfo.imageExtent );
	mCleaner.push( [=]() { mRenderpass->cleanup(); } );

	mCommandBuffers = cmdManager->createCommandBuffers( mTotalFrame );
	mPrepareSemaphores.resize( mTotalFrame );
	mSubmitSemaphores.resize( mTotalFrame );
	mSubmitFences.resize( mTotalFrame );
	
	VkSemaphoreCreateInfo semaphoreInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	VkFenceCreateInfo fenceInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr, VK_FENCE_CREATE_SIGNALED_BIT };
	
	for (u32 i = 0; i < mTotalFrame; i++) {
		vkCreateFence( device, &fenceInfo, nullptr, &mSubmitFences[i] );
		vkCreateSemaphore( device, &semaphoreInfo, nullptr, &mPrepareSemaphores[i] );
		vkCreateSemaphore( device, &semaphoreInfo, nullptr, &mSubmitSemaphores[i] );

		mCleaner.push([=](){ vkDestroyFence(device, mSubmitFences[i], nullptr); });
		mCleaner.push([=](){ vkDestroySemaphore(device, mPrepareSemaphores[i], nullptr); });
		mCleaner.push([=](){ vkDestroySemaphore(device, mSubmitSemaphores[i], nullptr); });
	}
	mCleaner.push([=]() { System::Device()->waitQueueIdle(); });
}

void Swapchain::prepareFrame() {
	VkDevice device = System::Device()->getDevice();
	VkFence submitFence = mSubmitFences[mActiveIdx];
	VkSemaphore prepareSemaphore = mPrepareSemaphores[mActiveIdx];
	VkResult result = vkAcquireNextImageKHR( device, mSwapchain,
											 UINT32_MAX, prepareSemaphore,
											 VK_NULL_HANDLE, &mActiveIdx );
	checkSwapchainResult( result );
	vkWaitForFences( device, 1, &submitFence, VK_TRUE, UINT32_MAX );
	mRenderpass->useFrame( mActiveIdx );
}

void Swapchain::submitFrame() {
	VkDevice device = System::Device()->getDevice();
	VkQueue graphicQueue = System::Device()->getGraphicQueue();
	VkFence submitFence  = mSubmitFences[mActiveIdx];
	VkSemaphore prepareSemaphore = mPrepareSemaphores[mActiveIdx];
	VkSemaphore submitSemaphore  = mSubmitSemaphores[mActiveIdx];
	VkCommandBuffer cmdBuffer    = mCommandBuffers[mActiveIdx];
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pWaitDstStageMask    = waitStages;
	submitInfo.waitSemaphoreCount   = 1;
	submitInfo.pWaitSemaphores      = &prepareSemaphore;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores    = &submitSemaphore;
	submitInfo.commandBufferCount   = 1;
	submitInfo.pCommandBuffers      = &cmdBuffer;

	vkResetFences( device, 1, &submitFence );
	VkResult result = vkQueueSubmit(graphicQueue, 1, &submitInfo, submitFence);
	ASSERT_VKERROR(result, "failed to submit draw command buffer!");
}

void Swapchain::presentFrame() {
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.swapchainCount     = 1;
	presentInfo.pSwapchains        = &mSwapchain;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores    = &mSubmitSemaphores[mActiveIdx];
	presentInfo.pImageIndices      = &mActiveIdx;

	VkQueue presentQueue = System::Device()->getPresentQueue();
	VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);
	checkSwapchainResult( result );
}

RpPresent* Swapchain::getRenderpass() { return mRenderpass; }

void Swapchain::checkSwapchainResult(VkResult result) {
	if (result != VK_ERROR_OUT_OF_DATE_KHR && result != VK_SUBOPTIMAL_KHR) { return; }
	LOG("swap chain failed!");
	cleanup();
	create( mSwapchainInfo.surface );
	createFrames();
}
