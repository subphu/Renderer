#include "rpPresent.h"
#include <system.h>
#include <resources/frame.h>

RpPresent::~RpPresent() {}
RpPresent::RpPresent() {}

void RpPresent::useFrame( const u32 idx ) { mFrame = mFrames[idx]; }

void RpPresent::createFrames( const vector<VkImage> images, const UInt2 size ) {
	VkFormat attachmentFormat = System::Device()->getSurfaceFormat().format;
	for (const VkImage image : images) {
		Frame* frame = new Frame( size );
		frame->addColorOutput( image, attachmentFormat );
		frame->createFramebuffer( mRenderpass );
		mFrames.push_back( frame );
		mCleaner.push([=](){ frame->cleanup(); });
	}
}

void RpPresent::setupAttachment() {
	VkFormat attachmentFormat = System::Device()->getSurfaceFormat().format;
	VkAttachmentDescription attachmentDesc = GetDefaultAttachmentDesc();
	attachmentDesc.format		 = attachmentFormat;
	attachmentDesc.finalLayout	 = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	addAttachment( attachmentDesc, { VEC4_BLACK } );
}

void RpPresent::setupSubpass() {
	mSubpasses.push_back( {} );
	mSubpasses.back().colorAttachmentRefs.push_back({0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
	mSubpasses.back().setupDesc();
}

void RpPresent::setupDependency() {
	VkSubpassDependency dependency{};
	dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
	dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstSubpass    = 0;
	dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	mDependencies.push_back( dependency );
}
