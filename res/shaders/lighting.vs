#version 410 core

// #ifdef GL_ES
// // Set default precision to medium
// precision mediump int;
// precision mediump float;
// // precision mediump vec2;
// #endif

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}