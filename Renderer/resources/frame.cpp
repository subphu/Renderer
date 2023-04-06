//  Copyright © 2021 Subph. All rights reserved.
//

#include "frame.h"
#include <system.h>
#include <renderpass/renderpass.h>
#include <resources/image.h>

Frame::~Frame() {}
Frame::Frame( const UInt2D size ) : mSize( size ) {}

void Frame::cleanup() { mCleaner.flush("Frame"); }

void Frame::addDepthOutput() {
	mDepthOutput = new Image();
	mDepthOutput->setUsage( VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
							VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT );
	mDepthOutput->create( mSize, VK_FORMAT_D24_UNORM_S8_UINT );
	mDepthOutput->createSampler();
	mDepthOutput->allocateImageMemory();

	mAttachments.push_back( mDepthOutput->getImageView() );
	mCleaner.push( [=]() { mDepthOutput->cleanup(); mAttachments.pop_back(); } );
}

void Frame::addColorOutput( const VkFormat format ) {
	Image* image = new Image();
	image->setUsage( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
					 VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT );
	image->create( mSize, format );
	image->createSampler();
	image->allocateImageMemory();

	mColorOutput.push_back( image );
	mAttachments.push_back( image->getImageView() );
	mCleaner.push( [=]() { image->cleanup(); mColorOutput.pop_back(); mAttachments.pop_back(); } );
}

void Frame::addColorOutput( const VkImage vkImage, const VkFormat format ) {
	Image* image = new Image();
	image->create( vkImage, format );

	mColorOutput.push_back( image );
	mAttachments.push_back( image->getImageView() );
	mCleaner.push( [=]() { image->cleanup(); mColorOutput.pop_back(); mAttachments.pop_back(); } );
}

void Frame::createFramebuffer( const VkRenderPass renderpass ) {
	LOG("createFramebuffer");
	const VkDevice device = System::Device()->getDevice();
	
	mFramebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	mFramebufferInfo.renderPass      = renderpass;
	mFramebufferInfo.attachmentCount = U32(mAttachments.size());
	mFramebufferInfo.pAttachments    = mAttachments.data();
	mFramebufferInfo.width           = mSize.width;
	mFramebufferInfo.height          = mSize.height;
	mFramebufferInfo.layers          = mLayer;

	VkResult result = vkCreateFramebuffer( device, &mFramebufferInfo, nullptr, &mFramebuffer );
	ASSERT_VKERROR(result, "failed to create framebuffer!");
	mCleaner.push( [=]() { vkDestroyFramebuffer( device, mFramebuffer, nullptr ); } );
}

const UInt2D& Frame::getSize() { return mSize; }
const VkFramebuffer Frame::getFramebuffer() { return mFramebuffer; }

const Image* Frame::getDepthOutput() { return mDepthOutput; }
const vector<Image*>& Frame::getColorOutput() { return mColorOutput;}

const u32 Frame::getAttachmentSize() { return U32( mAttachments.size() ); }

void Frame::setSize(UInt2D size) { mSize = size; }
