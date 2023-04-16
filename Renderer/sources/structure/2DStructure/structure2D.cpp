#include "structure2D.h"

#include <system.h>
#include <core/swapchain.h>
#include <resources/frame.h>

Structure2D::~Structure2D() {}
Structure2D::Structure2D() {
	mResolveLayer.renderpass = new RpResolve2D();
	mResolveLayer.pipelines.push_back( new PlResolve2D( mResolveLayer.renderpass, 0));
}

void Structure2D::setup() {
	mResolveLayer.renderpass->create();
	mResolveLayer.renderpass->createFrame();
	for (Pipeline* pipeline : mResolveLayer.pipelines) {
		pipeline->setup();
		pipeline->createGraphicsPipeline();
	}
}

void Structure2D::update() {
	for (Pipeline* pipeline : mResolveLayer.pipelines) {
		pipeline->update();
	}
}

void Structure2D::draw() {
	Swapchain* swapchain = System::Swapchain();
	VkCommandBuffer cmdBuffer;

	swapchain->prepare( &cmdBuffer );

	mResolveLayer.renderpass->begin( cmdBuffer );
	for (Pipeline* pipeline : mResolveLayer.pipelines) {
		pipeline->draw( cmdBuffer );
	}
	mResolveLayer.renderpass->end( cmdBuffer );

	swapchain->present( mResolveLayer.renderpass->getFramePtr()->getColorOutput()[0] );
}

void Structure2D::cleanup() {
	mResolveLayer.renderpass->cleanup();
	for (Pipeline* pipeline : mResolveLayer.pipelines) {
		pipeline->cleanup();
	}

}



