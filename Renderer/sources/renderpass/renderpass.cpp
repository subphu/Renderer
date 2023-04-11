//  Copyright © 2021 Subph. All rights reserved.
//

#include "renderpass.h"
#include <system.h>
#include <resources/frame.h>

Renderpass::~Renderpass() {}
Renderpass::Renderpass() {}

void Renderpass::cleanup() { mCleaner.flush("Renderpass"); }

void Renderpass::create() {
	setupAttachment();
	setupSubpass();
	setupDependency();
	createRenderpass();
}

void Renderpass::createRenderpass() {
	vector<VkSubpassDescription> subpasses;
	for (Subpass subpass : mSubpasses) {
		subpasses.push_back( subpass.subpassDesc );
	}

	mRenderpassInfo.sType            = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	mRenderpassInfo.attachmentCount  = U32( mAttachmentDescs.size() );
	mRenderpassInfo.pAttachments     = mAttachmentDescs.data();
	mRenderpassInfo.subpassCount     = U32( subpasses.size() );
	mRenderpassInfo.pSubpasses       = subpasses.data();
	mRenderpassInfo.dependencyCount  = U32( mDependencies.size() );
	mRenderpassInfo.pDependencies    = mDependencies.data();

	VkDevice device = System::Device()->getDevice();
	VkResult result = vkCreateRenderPass(device, &mRenderpassInfo, nullptr, &mRenderpassPtr);
	ASSERT_VKERROR(result, "failed to create render pass!");
	mCleaner.push([=](){ vkDestroyRenderPass(device, mRenderpassPtr, nullptr); });
}

void Renderpass::begin( const VkCommandBuffer cmdBuffer ) {
	VkRenderPassBeginInfo renderBeginInfo{};
	renderBeginInfo.sType			  = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderBeginInfo.renderPass		  = mRenderpassPtr;
	renderBeginInfo.clearValueCount	  = U32( mClearValues.size() );
	renderBeginInfo.pClearValues	  = mClearValues.data();
	renderBeginInfo.framebuffer		  = mFrame->getFramebuffer();
	renderBeginInfo.renderArea.extent = mFrame->getSize();
	renderBeginInfo.renderArea.offset = { 0, 0 };
	
	vkCmdBeginRenderPass(cmdBuffer, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void Renderpass::end( const VkCommandBuffer cmdBuffer ) {
	vkCmdEndRenderPass( cmdBuffer );
}

VkRenderPass Renderpass::get() { return mRenderpassPtr; }

Frame* Renderpass::getFramePtr() { return mFrame; }

const u32 Renderpass::addAttachment( const VkAttachmentDescription attachmentDesc, const VkClearValue clearValue ) {
	u32 idx = U32( mAttachmentDescs.size() );
	mAttachmentDescs.push_back( attachmentDesc );
	mClearValues.push_back( clearValue );
	return idx;
}

VkAttachmentDescription Renderpass::GetDefaultAttachmentDesc( const VkFormat format ) {
	VkAttachmentDescription attachment{};
	attachment.format         = format;
	attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
	attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment.finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	return attachment;
}

VkAttachmentDescription Renderpass::GetDefaultDepthAttachmentDesc() {
	VkAttachmentDescription attachment{};
	attachment.format         = VK_FORMAT_D24_UNORM_S8_UINT;
	attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
	attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	return attachment;
}

VkClearValue Renderpass::GetDefaultClearValue() {
	VkClearValue clearValue;
	clearValue.color = VEC4_BLACK;
	clearValue.depthStencil = { 0, 0 };
	return clearValue;
}
