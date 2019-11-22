#version 410
layout(location = 0) in vec3 vertex_pos;
layout(location = 1) in vec3 vertex_color;
layout(location = 2) in vec2 vextex_uv;

out vec3 color;
out vec2 texUV;

uniform mat4 MVP;

void main() { 
	color = vertex_color;
	vec3 vertex = vertex_pos;
	vertex.z = 0;

	gl_Position = MVP * vec4(vertex, 1.0);
	texUV = vextex_uv;
}
	