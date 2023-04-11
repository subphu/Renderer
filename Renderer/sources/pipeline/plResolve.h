#pragma once

#include "pipeline.h"

class Renderpass;

class PlResolve : public Pipeline {

public:
	~PlResolve();
	PlResolve( Renderpass* renderpass, const u32 subpass );

	void setupPipelineLayout() override;
	void setupShaderStages() override;
	void setupVertexInputInfo() override;

	void update() override;
	void draw( VkCommandBuffer cmdBuffer ) override;
};

