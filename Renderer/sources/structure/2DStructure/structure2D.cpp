#include "structure2D.h"

#include <system.h>
#include <core/swapchain.h>
#include <resources/frame.h>

Structure2D::~Structure2D() {}
Structure2D::Structure2D() {
	mResolveLayer.renderpass = new RpResolve2D();
	mResolveLayer.pipelines.push_back( new PlResolve2D( mResolveLayer.renderpass, 0 ) );
}

void Structure2D::setup() {
	mResolveLayer.renderpass->create();
	mResolveLayer.renderpass->createFrame();
	for (Pipeline* pipeline : mResolveLayer.pipelines) {
		pipeline->setup();
		pipeline->createGraphicsPipeline();
	}
}

void Structure2D::update( RenderTime renderTime ) {
	for (Pipeline* pipeline : mResolveLayer.pipelines) {
		pipeline->update( renderTime );
	}
}

void Structure2D::draw() {
	setupViewportScissor();
	Swapchain* swapchain = System::Swapchain();

	VkCommandBuffer cmdBuffer;
	if (!swapchain->prepare( &cmdBuffer ))
		return refresh();

	mResolveLayer.renderpass->begin( cmdBuffer );

	vkCmdSetViewport( cmdBuffer, 0, 1, &mViewport );
	vkCmdSetScissor( cmdBuffer, 0, 1, &mScissor );
	
	for (Pipeline* pipeline : mResolveLayer.pipelines) {
		pipeline->draw( cmdBuffer );
	}

	mResolveLayer.renderpass->end( cmdBuffer );

	Image* image = mResolveLayer.renderpass->getFramePtr()->getColorOutput()[0];
	if (!swapchain->present( image ))
		return refresh();
}

void Structure2D::refresh() {
	mResolveLayer.renderpass->recreateFrame();
}

void Structure2D::cleanup() {
	mResolveLayer.renderpass->cleanup();
	for (Pipeline* pipeline : mResolveLayer.pipelines) {
		pipeline->cleanup();
	}

}



