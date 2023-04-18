#include "rpResolve.h"
#include <system.h>
#include <resources/frame.h>

RpResolve::~RpResolve() {}
RpResolve::RpResolve() {}

void RpResolve::setupAttachment() {
	VkAttachmentDescription attachmentDesc = GetDefaultAttachmentDesc();
	attachmentDesc.format = VK_FORMAT_R8G8B8A8_UNORM;
	attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	addAttachment( attachmentDesc, { VEC4_BLACK } );
	
}

void RpResolve::setupSubpass() {
	mSubpasses.push_back( {} );
	mSubpasses.back().colorAttachmentRefs.push_back( { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } );
	mSubpasses.back().setupDesc();
}

void RpResolve::setupDependency() {
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstSubpass = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	mDependencies.push_back( dependency );
}

void RpResolve::createFrame() {
	mFrame = new Frame( System::Window()->getFrameSize() );
	mFrame->addColorOutput( VK_FORMAT_R8G8B8A8_UNORM );
	mFrame->createFramebuffer( mRenderpassPtr );
	mCleaner.push( [=]() { mFrame->cleanup(); } );
}