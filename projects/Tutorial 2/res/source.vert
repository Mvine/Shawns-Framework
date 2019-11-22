#version 410 core
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec4 inUv;
layout (location = 2) in vec3 inNormals;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

layout(location = 0) out vec3 outNormals; 

void main() 
{
	gl_Position = projection * view * model * vec4(inPosition, 1);
	outNormals = inNormals;

}