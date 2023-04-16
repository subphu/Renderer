#include "defaultStructure.h"

#include <system.h>
#include <core/swapchain.h>
#include <resources/frame.h>

DefaultStructure::~DefaultStructure() {}
DefaultStructure::DefaultStructure() {
	mResolveLayer.renderpass = new RpResolve();
	mResolveLayer.pipelines.push_back( new PlResolve( mResolveLayer.renderpass, 0));
}

void DefaultStructure::setup() {
	mResolveLayer.renderpass->create();
	mResolveLayer.renderpass->createFrame();
	for (Pipeline* pipeline : mResolveLayer.pipelines) {
		pipeline->setup();
		pipeline->createGraphicsPipeline();
	}
}

void DefaultStructure::update() {
	for (Pipeline* pipeline : mResolveLayer.pipelines) {
		pipeline->update();
	}
}

void DefaultStructure::draw() {
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

void DefaultStructure::cleanup() {
	mResolveLayer.renderpass->cleanup();
	for (Pipeline* pipeline : mResolveLayer.pipelines) {
		pipeline->cleanup();
	}

}



