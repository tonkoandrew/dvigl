#version 410 core
layout(location = 0) in vec3 attr_pos;
layout(location = 1) in vec3 attr_normal;
layout(location = 2) in vec2 attr_texcoord;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(attr_pos, 1.0);
}