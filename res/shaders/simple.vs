#version 410 core

// #ifdef GL_ES
// // Set default precision to medium
// precision mediump int;
// precision mediump float;
// // precision mediump vec2;
// #endif

layout(location = 0) in vec4 attr_pos;
layout(location = 1) in vec3 attr_normal;
layout(location = 2) in vec3 attr_tangent;
layout(location = 3) in vec2 attr_texcoord;

out vec2 v_texcoord;
out vec4 v_pos;

out vec3 v_normal;
out vec3 v_tangent;

uniform mat4 mvp;
uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;

void main()
{
    v_texcoord = attr_texcoord;
    v_pos = model * attr_pos;
    v_normal = (mvp * vec4(attr_normal, 0.0)).xyz;
    v_tangent = (mvp * vec4(attr_tangent, 0.0)).xyz;

    gl_Position = mvp * attr_pos;
}
