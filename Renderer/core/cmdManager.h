//  Copyright © 2021 Subph. All rights reserved.
//

#pragma once

#include <common.h>

class CmdManager {

public:
	~CmdManager();
	CmdManager();

	void cleanup();

	void createPool();

	VkCommandBuffer createCommandBuffer();
	vector<VkCommandBuffer> createCommandBuffers( const u32 size );

	void beginSingleTimeCommands( const VkCommandBuffer commandBuffer );
	void endSingleTimeCommands( const VkCommandBuffer commandBuffer );


private:
	Cleaner mCleaner{};

	VkCommandPoolCreateInfo mPoolInfo{};
	VkCommandPool mCommandPool = VK_NULL_HANDLE;

};
