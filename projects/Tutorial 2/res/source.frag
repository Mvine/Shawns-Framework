#version 410

layout(location = 0)in vec3 inNormals;

layout(location = 0)out vec4 outColor;

 void main() {
 outColor = vec4(inNormals, 1.0f);
 }
