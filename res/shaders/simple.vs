#version 330

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

layout (location = 0) in vec4 attr_pos;
layout (location = 1) in vec3 attr_normal;
layout (location = 2) in vec3 attr_tangent;
layout (location = 3) in vec2 attr_texcoord;


out vec2 v_texcoord;
out vec3 v_normal;
out vec4 pos;

uniform mat4 mvp;

void main()
{
  pos = attr_pos;
  v_texcoord = attr_texcoord;
  v_normal = attr_normal;
  gl_Position = mvp * attr_pos;
}
