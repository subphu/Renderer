#version 450

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 fragColor;
layout(binding = 0) uniform sampler2D prevFrame;

layout(push_constant) uniform FrameInfo {
	uvec2 resolution;
	float framerate;
	uint frame;
	uint microsecond;
	uint nanosecond;
	vec4 mousePos;
	uvec2 mouseState;
};

vec2 getCoord() {
	return uv * resolution;
}

vec2 getSpace() {
	vec2 coord = uv * 2.0 - 1.0;
	return vec2(coord.x, -coord.y);
}

void main() {
	fragColor = vec4(uv, 1.0,1.0);
}
