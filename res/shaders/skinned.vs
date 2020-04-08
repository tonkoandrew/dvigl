#version 410 core

layout(location = 0) in vec3 attr_pos;
layout(location = 1) in vec2 attr_texcoord;
layout(location = 2) in vec3 attr_normal;
layout(location = 3) in vec3 attr_tangent;
layout(location = 4) in ivec4 BoneIDs;
layout(location = 5) in vec4 Weights;

out vec2 v_texcoord;
out vec4 v_pos;
out vec3 v_normal;
out vec3 v_tangent;

const int MAX_BONES = 100;

uniform mat4 mvp;
uniform mat4 model;

uniform mat4 gBones[MAX_BONES];

void main()
{
    v_texcoord = attr_texcoord;

    mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
    BoneTransform += gBones[BoneIDs[1]] * Weights[1];
    BoneTransform += gBones[BoneIDs[2]] * Weights[2];
    BoneTransform += gBones[BoneIDs[3]] * Weights[3];

    mat4 mvpb = mvp * BoneTransform;
    v_pos = model * (BoneTransform * vec4(attr_pos, 1.0));
    v_normal = (mvpb * vec4(attr_normal, 0.0)).xyz;
    v_tangent = (mvpb * vec4(attr_tangent, 0.0)).xyz;
    gl_Position = mvpb * vec4(attr_pos, 1.0);
}
