#include "rpPresent.h"
#include <system.h>
#include <resources/frame.h>

RpPresent::~RpPresent() {}
RpPresent::RpPresent() {}

void RpPresent::setActiveFrame( const u32 idx ) { mFrame = mFrames[idx]; }

void RpPresent::createFrames( const vector<VkImage> images, const UInt2 size ) {
	VkFormat attachmentFormat = System::Device()->getSurfaceFormat().format;
	for (const VkImage image : images) {
		Frame* frame = new Frame( size );
		frame->addColorOutput( image, attachmentFormat );
		frame->createFramebuffer( mRenderpass );
	}
}


void RpPresent::setupAttachment() {
	VkFormat attachmentFormat = System::Device()->getSurfaceFormat().format;
	VkAttachmentDescription attachmentDesc = GetDefaultAttachmentDesc();
	attachmentDesc.format		 = attachmentFormat;
	attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	attachmentDesc.finalLayout	 = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	addAttachment( attachmentDesc, { VEC4_BLACK } );
}

void RpPresent::setupSubpass() {
	Subpass presentSubpass;
	presentSubpass.colorAttachmentRefs.push_back( { 0, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR } );
	presentSubpass.setupDesc();

	mSubpasses.push_back( presentSubpass );
}

void RpPresent::setupDependency() {
	VkSubpassDependency dependency;
	dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
	dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstSubpass    = 0;
	dependency.dstStageMask  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	mDependencies.push_back( dependency );
}