#include "plResolve2D.h"

#include <system.h>
#include <core/renderpass.h>
#include <core/descriptor.h>
#include <resources/shader.h>
#include <resources/frame.h>
#include <resources/image.h>

PlResolve2D::~PlResolve2D() {}
PlResolve2D::PlResolve2D( Renderpass* renderpass, const u32 subpass ) :
	Pipeline( renderpass, subpass ) {}

void PlResolve2D::setupShaderStages() {
	createShaderStages();
	System::Watcher()->addShaderFile( mVertShader->getFilepath(), [=]() { shaderUpdateCallback(); } );
	System::Watcher()->addShaderFile( mFragShader->getFilepath(), [=]() { shaderUpdateCallback(); } );
}

void PlResolve2D::createShaderStages() {
	mVertShader = new Shader( "fill.vert", VK_SHADER_STAGE_VERTEX_BIT );
	mFragShader = new Shader( "fill.frag", VK_SHADER_STAGE_FRAGMENT_BIT );

	mShaderStages = { mVertShader->getShaderStageInfo(), mFragShader->getShaderStageInfo() };
	mCleaner.push( [=]() { mVertShader->cleanup(); mFragShader->cleanup(); } );
}

void PlResolve2D::setupDescriptorSet() {
	mValid = true;
}

void PlResolve2D::setupPipelineLayout() {
	LOG( "PlResolve2D::setupPipelineLayout()" );
	VkDevice device = System::Device()->getDevice();
	
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.size = sizeof(FrameInfo);
    pushConstantRange.offset = 0;
    pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

	VkResult result = vkCreatePipelineLayout( device, &pipelineLayoutInfo, nullptr, &mPipelineLayout );
	mCleaner.push( [=]() { vkDestroyPipelineLayout( device, mPipelineLayout, nullptr ); } );
}

void PlResolve2D::setupVertexInputInfo() {
	mVertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
}

void PlResolve2D::update( const RenderTime renderTime ) {
	mFrameInfo.resolution	  = System::Window()->getFrameSize();
	mFrameInfo.framerate	  = renderTime.framerate;
	mFrameInfo.frame		  = renderTime.frameNum;
	mFrameInfo.microsecond	  = renderTime.microsecond;
	mFrameInfo.millisecond	  = renderTime.millisecond;
	mFrameInfo.mouseState	  = System::Window()->getMouseState();
	mFrameInfo.mousePos		  = System::Window()->getMousePos();
	mFrameInfo.mouseDownDelta = System::Window()->getMouseDownDelta();
}

void PlResolve2D::draw( VkCommandBuffer cmdBuffer ) {
	if (!mValid) return;

 	vkCmdPushConstants( cmdBuffer, mPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof( FrameInfo ), &mFrameInfo );

	vkCmdBindPipeline( cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline );
	vkCmdDraw( cmdBuffer, 3, 1, 0, 0 );
}

void PlResolve2D::shaderUpdateCallback() {
	LOG( "PlResolve2D::shaderUpdateCallback()" );
	System::Device()->waitQueueIdle();
	cleanup();
	createShaderStages();
	setupPipelineLayout();
	createGraphicsPipeline();
}
