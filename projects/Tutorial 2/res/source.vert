#version 410 core
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec4 inColor;
layout (location = 0) out vec4 outColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


void main() 
{
	gl_Position = projection * view * model * vec4(inPosition, 1);
	outColor = inColor;
}