#version 410 core

// #ifdef GL_ES
// // Set default precision to medium
// precision mediump int;
// precision mediump float;
// // precision mediump vec2;
// #endif

layout (location = 0) in vec4 attr_pos;
layout (location = 1) in vec3 attr_normal;
layout (location = 2) in vec3 attr_tangent;
layout (location = 3) in vec2 attr_texcoord;


out vec2 v_texcoord;
out vec3 v_normal;
out vec4 v_pos;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec4 worldPos = model * attr_pos;

    v_pos = worldPos;
    v_texcoord = attr_texcoord;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    v_normal = normalMatrix * attr_normal;

    gl_Position = projection * view * worldPos;

    vec3 N = v_normal;
    vec3 T = normalMatrix * attr_tangent;
    vec3 B = cross(N, T);

    TBN = transpose(mat3(T, B, N));
}
