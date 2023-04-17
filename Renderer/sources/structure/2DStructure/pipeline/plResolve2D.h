#pragma once

#include <core/pipeline.h>

class Renderpass;

class PlResolve2D : public Pipeline {

public:
	~PlResolve2D();
	PlResolve2D( Renderpass* renderpass, const u32 subpass );

	void setupShaderStages() override;
	void setupDescriptorSet() override;
	void setupPipelineLayout() override;
	void setupVertexInputInfo() override;

	void update( RenderTime renderTime ) override;
	void draw( VkCommandBuffer cmdBuffer ) override;

private:
	
	struct FrameInfo {
		UInt2 resolution;
		f32 framerate;
		u32 frame;
		u32 microsecond;
		u32 nanosecond;
		vec4 mousePos;
		UInt2 mouseState;
	};

	FrameInfo mFrameInfo{};
};

