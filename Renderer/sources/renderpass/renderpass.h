//  Copyright © 2021 Subph. All rights reserved.
//

#pragma once

#include <common.h>

class Frame;

class Renderpass {

public:
	struct Subpass {
		VkSubpassDescription subpassDesc{};
		VkAttachmentReference* depthAttachmentRef;
		vector<VkAttachmentReference> colorAttachmentRefs;
		vector<VkAttachmentReference> inputAttachmentRefs;
		vector<u32> preserveAttachmentRefs;

		void setupDesc() {
			subpassDesc.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDesc.pDepthStencilAttachment = depthAttachmentRef;
			subpassDesc.pColorAttachments		= colorAttachmentRefs.data();
			subpassDesc.colorAttachmentCount	= U32( colorAttachmentRefs.size() );
			subpassDesc.pInputAttachments		= inputAttachmentRefs.data();
			subpassDesc.inputAttachmentCount	= U32( inputAttachmentRefs.size() );
			subpassDesc.pPreserveAttachments	= preserveAttachmentRefs.data();
			subpassDesc.preserveAttachmentCount = U32( preserveAttachmentRefs.size() );
		}
	};
	
public:
	~Renderpass();
	Renderpass();
	
	void cleanup();
	void create();

	virtual void setupAttachment() = 0;
	virtual void setupSubpass()	   = 0;
	virtual void setupDependency() = 0;
	virtual void createFrame()	   = 0;

	void createRenderpass();

	void begin( const VkCommandBuffer cmdBuffer );
	void end( const VkCommandBuffer cmdBuffer );
	
	VkRenderPass get();
	Frame* getFramePtr();

protected:
	Cleaner mCleaner;

	vector<VkClearValue> mClearValues;
	vector<VkAttachmentDescription> mAttachmentDescs;

	vector<Subpass> mSubpasses;
	vector<VkSubpassDependency> mDependencies;

	Frame* mFrame;

	VkRenderPass mRenderpassPtr;
	VkRenderPassCreateInfo mRenderpassInfo{};
	
	const u32 addAttachment( const VkAttachmentDescription attachmentDesc,
							 const VkClearValue clearValue = GetDefaultClearValue() );
		
	static VkAttachmentDescription GetDefaultAttachmentDesc( const VkFormat format = VK_FORMAT_R8G8B8A8_UNORM );
	static VkAttachmentDescription GetDefaultDepthAttachmentDesc();
	static VkClearValue GetDefaultClearValue();
};
