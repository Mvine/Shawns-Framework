#version 410

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out vec3 outColor;
layout(location = 2) out vec3 outNormal;

out vec3 CamDirection;
out vec3 LightDirection;

uniform mat4 MVP;
uniform mat4 View;
uniform mat4 Model;
uniform vec3 LightWorldPos;

void main() 
{
	gl_Position = MVP * vec4(inPosition, 1.0f);

	//pass vertex position to frag shader
	outWorldPos = (Model * vec4(inPosition, 1.0f)).xyz;

	//we need camera direction
	vec3 vertexPos = (View * Model * vec4(inPosition, 1.0f)).xyz;
	CamDirection   = vec3(0.0f, 0.0f, 0.0f) - vertexPos;

	//light direction
	vec3 lightPos  = (View * Model * vec4(LightWorldPos, 1.0f)).xyz;
	LightDirection = lightPos + CamDirection;

	//normals
	outNormal = (View * Model * vec4(inNormal, 1.0f)).xyz;
} 