#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

attribute vec4 attr_pos;
attribute vec3 attr_normal;
attribute vec3 attr_tangent;
attribute vec2 attr_texcoord;

varying vec2 v_texcoord;
varying vec3 v_normal;

uniform mat4 mvp;
varying vec4 pos;

void main()
{
  pos = attr_pos;
  v_texcoord = attr_texcoord;
  v_normal = attr_normal;
  gl_Position = mvp * attr_pos;
}
