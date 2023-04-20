#pragma once

#include <core/pipeline.h>

class Renderpass;
class Shader;

class PlResolve2D : public Pipeline {

public:
	~PlResolve2D();
	PlResolve2D( Renderpass* renderpass, const u32 subpass );

	void createShaderStages();
	void setupShaderStages() override;
	void setupDescriptorSet() override;
	void setupPipelineLayout() override;
	void setupVertexInputInfo() override;

	void update( RenderTime renderTime ) override;
	void draw( VkCommandBuffer cmdBuffer ) override;

	void shaderUpdateCallback();

private:
	
	struct FrameInfo {
		UInt2 resolution;
		f32 framerate;
		u32 frame;
		u32 microsecond;
		u32 millisecond;
		uvec2 mouseState;
		vec4 mousePos;
		vec4 mouseDownDelta;
	};

	FrameInfo mFrameInfo{};
	Shader* mVertShader;
	Shader* mFragShader;
};

