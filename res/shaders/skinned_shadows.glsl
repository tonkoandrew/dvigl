#defshader vertex
#version 410 core
layout(location = 0) in vec3 attr_pos;
layout(location = 1) in vec3 attr_normal;
layout(location = 2) in vec2 attr_texcoord;
layout(location = 3) in vec3 attr_tangent;
layout(location = 4) in vec3 attr_bitangent;
layout(location = 5) in ivec4 BoneIDs;
layout(location = 6) in vec4 Weights;

uniform mat4 light_mvp;
uniform mat4 gBones[100];

void main()
{
    mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
    BoneTransform += gBones[BoneIDs[1]] * Weights[1];
    BoneTransform += gBones[BoneIDs[2]] * Weights[2];
    BoneTransform += gBones[BoneIDs[3]] * Weights[3];

    vec3 v_pos = (light_mvp * (BoneTransform * vec4(attr_pos, 1.0))).xyz;
    gl_Position = vec4(v_pos, 1.0);
}

#defshader fragment
#version 410 core
void main()
{             
    // gl_FragDepth = gl_FragCoord.z;
}