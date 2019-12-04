#version 410
layout(location = 0) in vec3 vertex_pos;
layout(location = 1) in vec3 vertex_color;
layout(location = 2) in vec2 vextex_uv;

out vec3 color;
out vec2 texUV;

uniform mat4 MVP;

uniform sampler2D myTextureSampler;

uniform float time;

void main() {
	
	vec3 v = vertex_pos;
	v.y = texture(myTextureSampler, vextex_uv).r;
	//v.y = 0;

	//v.y += sin(5.0*v.x + time) * 0.25;

	gl_Position = MVP * vec4(v, 1.0);
	texUV = vextex_uv;
}
	