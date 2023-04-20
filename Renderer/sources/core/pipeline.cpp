//  Copyright © 2021 Subph. All rights reserved.
//

#include "pipeline.h"

#include <system.h>
#include <core/renderpass.h>

Pipeline::~Pipeline() {}
Pipeline::Pipeline( Renderpass* renderpass, const u32 subpass ) :
	mRenderpassPtr( renderpass ),
	mSubpass( subpass ),
	mValid( false) {}

void Pipeline::cleanup() {
	mValid = false;
	mCleaner.flush( "Pipeline" );
}

VkPipeline Pipeline::get() { return mPipeline; }

void Pipeline::setupDescriptorSet() { }

void Pipeline::setup() {
	setupShaderStages();
	setupDescriptorSet();
	setupPipelineLayout();
	setupVertexInputInfo();
	setupViewportInfo();
	setupInputAssemblyInfo();
	setupRasterizationInfo();
	setupMultisampleInfo();
	setupColorBlendInfo();
	setupDynamicInfo();
	mDynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
}

void Pipeline::setupViewportInfo() {
	UInt2 extent = System::Window()->getFrameSize();
	mViewport.x = 0.f;
	mViewport.y = 0.f;
	mViewport.width = F32( extent.width );
	mViewport.height = F32( extent.height );
	mViewport.minDepth = 0.f;
	mViewport.maxDepth = 1.f;
	mScissor.offset = { 0, 0 };
	mScissor.extent = extent;

	mViewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	mViewportInfo.viewportCount = 1;
	mViewportInfo.scissorCount  = 1;
	mViewportInfo.pViewports = &mViewport;
	mViewportInfo.pScissors = &mScissor;
}

void Pipeline::setupInputAssemblyInfo() {
	mInputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	mInputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

void Pipeline::setupRasterizationInfo() {
	mRasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	mRasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
	mRasterizationInfo.cullMode    = VK_CULL_MODE_BACK_BIT;
	mRasterizationInfo.frontFace   = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	mRasterizationInfo.lineWidth   = 1.0f;
}

void Pipeline::setupMultisampleInfo() {
	mMultisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	mMultisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
}

void Pipeline::setupColorBlendInfo( VkBool32 enable ) {
	mColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	mColorBlendAttachment.blendEnable         = enable;
	mColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	mColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	mColorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;
	mColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	mColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	mColorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;

	mColorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	mColorBlendInfo.attachmentCount = 1;
	mColorBlendInfo.pAttachments    = &mColorBlendAttachment;
}

void Pipeline::setupDepthStencilInfo(VkBool32 enable) {
	mDepthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	mDepthStencilInfo.depthTestEnable       = enable;
	mDepthStencilInfo.depthWriteEnable      = enable;
	mDepthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	mDepthStencilInfo.stencilTestEnable     = VK_FALSE;
	mDepthStencilInfo.depthCompareOp        = VK_COMPARE_OP_LESS;
}

void Pipeline::setupDynamicInfo() {
	mDynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	mDynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	mDynamicInfo.dynamicStateCount = U32( mDynamicStates.size() );
	mDynamicInfo.pDynamicStates = mDynamicStates.data();
}

void Pipeline::setupTessellationInfo() {
	mTessellationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
}

void Pipeline::createGraphicsPipeline() {
	VkDevice device = System::Device()->getDevice();
	
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.layout     = mPipelineLayout;
	pipelineInfo.renderPass = mRenderpassPtr->get();
	pipelineInfo.subpass    = mSubpass;
	pipelineInfo.stageCount = U32(mShaderStages.size());
	pipelineInfo.pStages             = mShaderStages.data();
	pipelineInfo.pVertexInputState   = &mVertexInputInfo;
	pipelineInfo.pViewportState      = &mViewportInfo;
	pipelineInfo.pInputAssemblyState = &mInputAssemblyInfo;
	pipelineInfo.pRasterizationState = &mRasterizationInfo;
	pipelineInfo.pMultisampleState   = &mMultisampleInfo;
	pipelineInfo.pColorBlendState    = &mColorBlendInfo;
	pipelineInfo.pDepthStencilState  = &mDepthStencilInfo;
	pipelineInfo.pDynamicState       = &mDynamicInfo;
	
	VkResult result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mPipeline );
	ASSERT_VKERROR(result, "failed to create graphics pipeline!");
	mCleaner.push([=](){ vkDestroyPipeline(device, mPipeline, nullptr); });
	mValid = true;
}

void Pipeline::createComputePipeline() {
	VkDevice device = System::Device()->getDevice();
	
	VkComputePipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineInfo.layout = mPipelineLayout;
	pipelineInfo.stage  = mShaderStages[0];
	
	VkResult result = vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mPipeline );
	ASSERT_VKERROR(result, "failed to create graphics pipeline!");
	mCleaner.push([=](){ vkDestroyPipeline(device, mPipeline, nullptr); });
}


