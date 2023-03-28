//  Copyright © 2021 Subph. All rights reserved.
//

#pragma once

#include <common.h>
#include <core/device.h>

class CmdManager {

public:
	~CmdManager();
	CmdManager();

	void cleanup();

	void createPool();

	VkCommandBuffer createCommandBuffer();
	vector<VkCommandBuffer> createCommandBuffers( u32 size );

	void beginSingleTimeCommands( VkCommandBuffer commandBuffer );
	void endSingleTimeCommands( VkCommandBuffer commandBuffer );


private:
	Cleaner mCleaner{};

	VkCommandPoolCreateInfo mPoolInfo{};
	VkCommandPool mCommandPool = VK_NULL_HANDLE;

};
