#include "plTriangle.h"

#include <system.h>
#include <core/renderpass.h>
#include <core/descriptor.h>
#include <resources/shader.h>

PlTriangle::~PlTriangle() {}
PlTriangle::PlTriangle( Renderpass* renderpass, const u32 subpass ) :
	Pipeline( renderpass, subpass ) {}

void PlTriangle::setupPipelineLayout() {
	VkDevice device = System::Device()->getDevice();

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;

	VkResult result = vkCreatePipelineLayout( device, &pipelineLayoutInfo, nullptr, &mPipelineLayout );
	mCleaner.push( [=]() { vkDestroyPipelineLayout( device, mPipelineLayout, nullptr ); } );
}

void PlTriangle::setupShaderStages() {
	Shader* vertShader = new Shader( "triangle.vert", VK_SHADER_STAGE_VERTEX_BIT );
	Shader* fragShader = new Shader( "triangle.frag", VK_SHADER_STAGE_FRAGMENT_BIT );
	mShaderStages = { vertShader->getShaderStageInfo(), fragShader->getShaderStageInfo() };
	mCleaner.push( [=]() { vertShader->cleanup(); fragShader->cleanup(); } );
}

void PlTriangle::setupVertexInputInfo() {
	mVertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
}

void PlTriangle::update() {}

void PlTriangle::draw( VkCommandBuffer cmdBuffer ) {
	vkCmdBindPipeline( cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline );
	vkCmdDraw( cmdBuffer, 3, 1, 0, 0 );
}
