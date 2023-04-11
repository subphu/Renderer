//  Copyright © 2021 Subph. All rights reserved.
//

#include "swapchain.h"
#include <system.h>
#include <resources/image.h>

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
	mSwapchainInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
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

	mImages = GetSwapchainImages(mSwapchain);
	mTotalFrame = U32( mImages.size() );

	mCmdBuffers = cmdManager->createCommandBuffers( mTotalFrame );
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

void Swapchain::prepare( VkCommandBuffer* cmdBuffer ) {
	VkDevice device = System::Device()->getDevice();

	VkFence submitFence = mSubmitFences[mImageIdx];
	vkWaitForFences( device, 1, &submitFence, VK_TRUE, UINT32_MAX );

	mSemaphoreIdx = (mSemaphoreIdx + 1) % mTotalFrame;

	VkSemaphore prepareSemaphore = mPrepareSemaphores[mSemaphoreIdx];
	VkResult result = vkAcquireNextImageKHR( device, mSwapchain,
											 UINT32_MAX, prepareSemaphore,
											 VK_NULL_HANDLE, &mImageIdx );
	checkSwapchainResult( result );

	*cmdBuffer = mCmdBuffers[mImageIdx];
	VkCommandBufferBeginInfo cmdBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	result = vkBeginCommandBuffer( *cmdBuffer, &cmdBeginInfo );
	ASSERT_VKERROR( result, "failed to start command buffer!" );
}

void Swapchain::present( Image* image ) {
	fillFrame( image );
	submitFrame();
	presentFrame();
}

const VkCommandBuffer Swapchain::getCmdBuffer() { return mCmdBuffers[mImageIdx]; }

void Swapchain::fillFrame( Image* image ) {
	VkCommandBuffer cmdBuffer = mCmdBuffers[mImageIdx];
	VkImage presentImage = mImages[mImageIdx];

	VkImageMemoryBarrier barrier = image->getMemoryBarrier();

	array<VkImageMemoryBarrier, 2> barriers{ barrier, barrier };
	barriers[0].newLayout	  = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	barriers[0].dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	barriers[1].image		  = presentImage;
	barriers[1].oldLayout	  = VK_IMAGE_LAYOUT_UNDEFINED;
	barriers[1].newLayout	  = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barriers[1].dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

	vkCmdPipelineBarrier( cmdBuffer,
						  VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
						  VK_PIPELINE_STAGE_TRANSFER_BIT,
						  0, 0, nullptr, 0, nullptr,
						  2, barriers.data() );

	image->copyTo( cmdBuffer, presentImage );
	
	barriers[0].oldLayout	  = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	barriers[0].newLayout	  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	barriers[1].oldLayout	  = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barriers[1].newLayout	  = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	
	vkCmdPipelineBarrier( cmdBuffer,
						  VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
						  VK_PIPELINE_STAGE_TRANSFER_BIT,
						  0, 0, nullptr, 0, nullptr,
						  2, barriers.data() );

	vkEndCommandBuffer( cmdBuffer );
}

void Swapchain::submitFrame() {
	VkCommandBuffer cmdBuffer = mCmdBuffers[mImageIdx];
	VkSemaphore prepareSemaphore = mPrepareSemaphores[mSemaphoreIdx];
	VkSemaphore submitSemaphore = mSubmitSemaphores[mSemaphoreIdx];
	VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount   = 1;
	submitInfo.pCommandBuffers      = &cmdBuffer;
	submitInfo.waitSemaphoreCount   = 1;
	submitInfo.pWaitSemaphores      = &prepareSemaphore;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores    = &submitSemaphore;
	submitInfo.pWaitDstStageMask    = &waitStages;

	VkDevice device = System::Device()->getDevice();
	VkQueue graphicQueue = System::Device()->getGraphicQueue();
	VkFence submitFence = mSubmitFences[mImageIdx];

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
	presentInfo.pWaitSemaphores    = &mSubmitSemaphores[mSemaphoreIdx];
	presentInfo.pImageIndices      = &mImageIdx;

	VkQueue presentQueue = System::Device()->getPresentQueue();
	VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);
	checkSwapchainResult( result );
}

void Swapchain::checkSwapchainResult(VkResult result) {
	if (result != VK_ERROR_OUT_OF_DATE_KHR && result != VK_SUBOPTIMAL_KHR) { return; }
	LOG("swap chain failed!");
	cleanup();
	create( mSwapchainInfo.surface );
	createFrames();
}
