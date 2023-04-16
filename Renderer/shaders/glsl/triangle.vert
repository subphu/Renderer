#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) out vec3 fragColor;

void main() {
	const float size = 0.5f;
	const vec3 positions[3] = vec3[3] (
		vec3(  1.f,  1.f, 0.0f ),
		vec3(  0.f, -1.f, 0.0f ),
		vec3( -1.f,  1.f, 0.0f )
	);
	const vec3 colors[3] = vec3[3] (
		vec3( 1.f, 0.f, 0.0f ),
		vec3( 0.f, 1.f, 0.0f ),
		vec3( 0.f, 0.f, 1.0f )
	);

	fragColor = colors[gl_VertexIndex];
	gl_Position = vec4(positions[gl_VertexIndex] * size, 1.0f);
}