#version 410

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 inNormal;

// New in tutorial 06
layout(location = 3) in vec2 inUV;

layout(location = 0) out vec4 outColor;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec3 outWorldPos;
layout (location = 3) out vec2 outUV;
layout (location = 4) out vec3 outTexWeights;

//for the height

uniform mat4 a_ModelViewProjection;
uniform mat4 a_ModelView;

uniform sampler2D s_HeightMap;


void main() {

	outColor = inColor;
	outNormal = inNormal;
	vec3 v = inPosition;
	v.z = texture(s_HeightMap, inUV).r * 4;

	// Write the output
	gl_Position = a_ModelViewProjection * vec4(v, 1.0);

	float height = v.z- 0.54f;

	//thanks shawn
	vec3 weights = vec3(
    clamp((-height + 0.2f) * 4.0f, 0.0f, 1.0f),
    min(clamp((-height + 1.95f) * 4.0f, 0.0f, 1.0f), clamp((height - 0.25f) * 4.0f, 0.0f, 1.0f)),
    clamp((height - 0.2f) * 4.0f, 0.0f, 1.0f));

	outTexWeights = weights;

 	outUV = inUV;

}