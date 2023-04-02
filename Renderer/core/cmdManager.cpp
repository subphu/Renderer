//  Copyright © 2021 Subph. All rights reserved.
//

#include "cmdManager.h"

#include <system.h>

CmdManager::~CmdManager() {}
CmdManager::CmdManager() {}

void CmdManager::cleanup() { mCleaner.flush( "CmdBuffer" ); }

void CmdManager::createPool() {
	LOG( "CmdBuffer::createPool" );
	const VkDevice device = System::Device()->getDevice();

	mPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	mPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	mPoolInfo.queueFamilyIndex = System::Device()->getGraphicQueueIndex();

	const VkResult result = vkCreateCommandPool( device, &mPoolInfo, nullptr, &mCommandPool );
	ASSERT_VKERROR( result, "failed to create command pool!" );
	mCleaner.push( [=]() { vkDestroyCommandPool( device, mCommandPool, nullptr ); } );
}

VkCommandBuffer CmdManager::createCommandBuffer() {
	return createCommandBuffers( 1 )[0];
}

vector<VkCommandBuffer> CmdManager::createCommandBuffers( const u32 size ) {
	LOG( "CmdBuffer::createCommandBuffers" );
	const VkDevice device = System::Device()->getDevice();

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = mCommandPool;
	allocInfo.commandBufferCount = size;

	vector<VkCommandBuffer> commandBuffers( size );
	const VkResult result = vkAllocateCommandBuffers( device, &allocInfo, commandBuffers.data() );
	ASSERT_VKERROR( result, "failed to allocate command buffer!" );
	return commandBuffers;
}

void CmdManager::beginSingleTimeCommands( const VkCommandBuffer commandBuffer ) {
	LOG( "CmdBuffer::beginSingleTimeCommands" );
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	const VkResult result = vkBeginCommandBuffer( commandBuffer, &beginInfo );
	ASSERT_VKERROR( result, "failed to start command!" );
}

void CmdManager::endSingleTimeCommands( const VkCommandBuffer commandBuffer ) {
	LOG( "CmdBuffer::endSingleTimeCommands" );
	const VkDevice device = System::Device()->getDevice();
	const VkQueue  queue  = System::Device()->getGraphicQueue();

	VkResult result = vkEndCommandBuffer( commandBuffer );
	ASSERT_VKERROR( result, "failed to end command!" );

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	result = vkQueueSubmit( queue, 1, &submitInfo, VK_NULL_HANDLE );
	ASSERT_VKERROR( result, "failed to submit command!" );

	result = vkQueueWaitIdle( queue );
	ASSERT_VKERROR( result, "failed to wait queue!" );

	vkFreeCommandBuffers( device, mCommandPool, 1, &commandBuffer );
}
