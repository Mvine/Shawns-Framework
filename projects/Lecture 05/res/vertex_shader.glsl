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
	gl_Position = MVP * vec4(inPosition, 1.0);

	//Pass vertex position to fragment shader 
	outWorldPos = (Model * vec4(inPosition, 1)).xyz;
	 
	//We need a camrea direction (sometimes known as eye / e)
	vec3 vertexPos = (View * Model * vec4(inPosition,1)).xyz; //origin - vertex looking at in CAMERA SPACE
	CamDirection = vec3(0.0, 0.0, 0.0) - vertexPos;

	//Light direction
	vec3 lightPos = (View * Model * vec4(LightWorldPos, 1)).xyz; 
	LightDirection = lightPos + CamDirection;

	//Normals
	outNormal = (View * Model * vec4(inNormal, 0)).xyz;

	outColor = inColor;
}
	