//  Copyright © 2021 Subph. All rights reserved.
//

#pragma once

#include <common.h>

class Renderpass;
class Image;

class Frame {
	
public:
	~Frame();
	Frame( const UInt2D size );
	
	void cleanup();

	void addDepthOutput();
	void addColorOutput( const VkFormat format );
	void addColorOutput( const VkImage image, const VkFormat format );
	void createFramebuffer( const VkRenderPass renderpass );

	const UInt2D& getSize();
	const VkFramebuffer getFramebuffer();
	
	const Image* getDepthOutput();
	const vector<Image*>& getColorOutput();
	
	const u32 getAttachmentSize();
	
	void setSize(UInt2D size);

private:
	Cleaner mCleaner;

	s32 mLayer = 1;
	UInt2D  mSize{};
	
	Image* mDepthOutput;
	vector<Image*> mColorOutput;
	vector<VkImageView> mAttachments;

	VkFramebufferCreateInfo mFramebufferInfo{};
	VkFramebuffer mFramebuffer = VK_NULL_HANDLE;
};
