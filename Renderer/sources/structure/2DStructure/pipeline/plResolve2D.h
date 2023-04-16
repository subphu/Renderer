#pragma once

#include <core/pipeline.h>

class Renderpass;

class PlResolve2D : public Pipeline {

public:
	~PlResolve2D();
	PlResolve2D( Renderpass* renderpass, const u32 subpass );

	void setupPipelineLayout() override;
	void setupShaderStages() override;
	void setupVertexInputInfo() override;

	void update() override;
	void draw( VkCommandBuffer cmdBuffer ) override;
};

