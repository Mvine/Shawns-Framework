#version 410

layout(location = 0)in vec3 inNormals;

layout(location = 0)out vec4 outColor;

uniform vec3 viewDirection;

 void main() {
 outColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);

 if(dot(normalize(inNormals), normalize(viewDirection)) < 0.5)
 {
	outColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
 }

 }
