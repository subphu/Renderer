#version 450

layout(location = 0) in vec2 fragUV;
layout(location = 0) out vec4 fragColor;
layout(binding = 0) uniform sampler2D prevFrame;

layout(push_constant) uniform FrameInfo {
	uvec2 resolution;
	float framerate;
	uint frame;
	uint microsecond;
	uint millisecond;
	uvec2 mouseState;
	vec4 mousePos;
	vec4 mouseDownDelta;
};

vec2 getUV() {
	return fragUV;
}

vec2 getCoord() {
	return getUV() * resolution;
}

vec2 getSpaceCoord() {
	vec2 coord = getCoord();
	coord = coord * 2.0 - resolution;
	coord = coord / resolution.y;
	return vec2(coord.x, -coord.y);
}

mat4 setRotation( float x, float y, float z ) {
	float a = sin(x); float b = cos(x); 
	float c = sin(y); float d = cos(y); 
	float e = sin(z); float f = cos(z); 

	float ac = a*c;
	float bc = b*c;

	return mat4( d*f,      d*e,       -c, 0.0,
				 ac*f-b*e, ac*e+b*f, a*d, 0.0,
				 bc*f+a*e, bc*e-a*f, b*d, 0.0,
				 0.0,      0.0,      0.0, 1.0 );
}

void main() {
	vec4 mouseDelta = mouseDownDelta / 100.0;
	vec3 camera = vec3(0.0, 0.0, 5.0 - mouseDelta.z);
	vec3 rayDir = normalize(vec3(getSpaceCoord(), -1.0));

	int rayCount = 32;
	float dist = 0.0;
	for (int i = 0; i < rayCount; ++i) {
		vec3 p = camera + dist * rayDir;
		vec3 rotP = (vec4(p, 1.0) * setRotation( mouseDelta.y, mouseDelta.x, 0.0)).xyz;
		float delta = length(max(abs(rotP) - vec3(1.0), 0.0));
		if (delta < 0.001) break;
		dist += delta;
	}

	vec3 color = vec3(dist / length(camera + 2.0));
	fragColor = vec4(1.0 - color, 1.0);
}
