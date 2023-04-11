//  Copyright © 2021 Subph. All rights reserved.
//

#pragma once

#include <common.h>

class Renderpass;

class Pipeline {
	
public:
	~Pipeline();
	Pipeline( Renderpass* renderpass, const u32 subpass );
	
	void cleanup();

	void setup();
	
	void setupViewportInfo();
	void setupInputAssemblyInfo();
	void setupRasterizationInfo();
	void setupMultisampleInfo();
	
	void setupColorBlendInfo( VkBool32 enable = VK_FALSE );
	
	// Optional
	void setupDepthStencilInfo( VkBool32 enable = VK_TRUE );
	void setupDynamicInfo();
	void setupTessellationInfo();

	void createComputePipeline();
	void createGraphicsPipeline();
	
	VkPipeline get();

	virtual void setupPipelineLayout() = 0;
	virtual void setupShaderStages() = 0;
	virtual void setupVertexInputInfo() = 0;

	virtual void update() = 0;
	virtual void draw( VkCommandBuffer cmdBuffer ) = 0;
	
protected:
	Cleaner mCleaner;

	vector<VkPipelineShaderStageCreateInfo> mShaderStages;
	VkPipelineVertexInputStateCreateInfo    mVertexInputInfo;

	VkPipelineViewportStateCreateInfo       mViewportInfo{};
	VkPipelineInputAssemblyStateCreateInfo  mInputAssemblyInfo{};
	VkPipelineTessellationStateCreateInfo   mTessellationInfo{};
	VkPipelineRasterizationStateCreateInfo  mRasterizationInfo{};
	VkPipelineMultisampleStateCreateInfo    mMultisampleInfo{};

	VkPipelineColorBlendAttachmentState     mColorBlendAttachment{};
	VkPipelineColorBlendStateCreateInfo     mColorBlendInfo{};

	vector<VkDynamicState>                  mDynamicStates;
	VkPipelineDynamicStateCreateInfo        mDynamicInfo{};
	VkPipelineDepthStencilStateCreateInfo   mDepthStencilInfo{};

	VkViewport mViewport{};
	VkRect2D   mScissor{};

	u32 mSubpass;
	Renderpass* mRenderpassPtr;
	VkPipelineLayout mPipelineLayout;
	VkPipeline mPipeline;
};
