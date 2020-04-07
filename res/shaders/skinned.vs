#version 410 core

layout (location = 0) in vec3 attr_pos;
layout (location = 1) in vec2 attr_texcoord;
layout (location = 2) in vec3 attr_normal;
layout (location = 3) in vec3 attr_tangent;
layout (location = 4) in ivec4 BoneIDs;
layout (location = 5) in vec4 Weights;

out vec2 v_texcoord;
out vec4 v_pos;
out mat3 TBN;

const int MAX_BONES = 100;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


uniform mat4 gBones[MAX_BONES];

void main()
{       
    mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
    BoneTransform     += gBones[BoneIDs[1]] * Weights[1];
    BoneTransform     += gBones[BoneIDs[2]] * Weights[2];
    BoneTransform     += gBones[BoneIDs[3]] * Weights[3];


    vec4 worldPos = model * (BoneTransform * vec4(attr_pos, 1.0));

    gl_Position  = projection * view * worldPos;

    v_pos = worldPos;
    v_texcoord   = attr_texcoord;

    vec4 NormalL = BoneTransform * vec4(attr_normal, 0.0);
    vec4 TangentL = BoneTransform * vec4(attr_tangent, 0.0);

    mat3 normalMatrix = transpose(inverse(mat3(model)));

    vec3 N = normalMatrix * NormalL.rgb;
    vec3 T = normalMatrix * TangentL.rgb;
    vec3 B = cross(N, T);

    TBN = transpose(mat3(T, B, N));
}
