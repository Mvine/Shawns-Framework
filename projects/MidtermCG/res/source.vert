#version 410 core
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec4 aColor;
uniform mat4 projection;
uniform mat4 model;

out vec4 ourColor;

void main() 
{
	gl_Position = projection * model * vec4(inPosition, 1);
	ourColor = aColor;
}