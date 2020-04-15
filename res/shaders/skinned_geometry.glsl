#defshader vertex
#version 410 core

layout(location = 0) in vec3 attr_pos;
layout(location = 1) in vec3 attr_normal;
layout(location = 2) in vec2 attr_texcoord;
layout(location = 3) in vec3 attr_tangent;
layout(location = 4) in vec3 attr_bitangent;
layout(location = 5) in ivec4 BoneIDs;
layout(location = 6) in vec4 Weights;

out vec2 v_texcoord;
out vec4 v_pos;
out mat3 TBN;

uniform mat4 mvp;
uniform mat4 model;
uniform mat3 normalMatrix;

uniform mat4 gBones[100];

void main()
{
    v_texcoord = attr_texcoord;

    mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
    BoneTransform += gBones[BoneIDs[1]] * Weights[1];
    BoneTransform += gBones[BoneIDs[2]] * Weights[2];
    BoneTransform += gBones[BoneIDs[3]] * Weights[3];

    mat4 mvpb = mvp * BoneTransform;
    v_pos = model * (BoneTransform * vec4(attr_pos, 1.0));

    vec4 norm = BoneTransform * vec4(attr_normal, 0.0);
    vec4 tang = BoneTransform * vec4(attr_tangent, 1.0);
    vec4 bitang = BoneTransform * vec4(attr_bitangent, 1.0);

    vec3 T = normalize(normalMatrix * tang.xyz);
    vec3 B = normalize(normalMatrix * bitang.xyz);
    vec3 N = normalize(normalMatrix * norm.xyz);
    TBN = mat3(T, B, N);

    // v_normal = (mvpb * vec4(attr_normal, 0.0)).xyz;
    // v_tangent = (mvpb * vec4(attr_tangent, 0.0)).xyz;

    gl_Position = mvpb * vec4(attr_pos, 1.0);
}

#defshader fragment
#version 410 core

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

in vec2 v_texcoord;
// in vec3 v_normal;
// in vec3 v_tangent;
in mat3 TBN;
in vec4 v_pos;

uniform sampler2D normalMap;
uniform sampler2D albedoMap;

// out vec4 FragColor;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec4 out_albedoSpec;
layout(location = 3) out vec4 out_depth;

void main()
{
    vec4 color = texture(albedoMap, v_texcoord);
    out_position = v_pos.xyz;

    // vec3 Normal = normalize(v_normal);
    // vec3 Tangent = normalize(v_tangent);
    // Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    // vec3 Bitangent = cross(Tangent, Normal);
    vec3 BumpMapNormal = texture(normalMap, v_texcoord).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    // mat3 TBN = mat3(Tangent, Bitangent, Normal);
    out_normal = normalize(TBN * BumpMapNormal);

    out_albedoSpec.rgb = color.rgb;
    out_albedoSpec.a = 1.0;

    out_depth = gl_FragCoord;
}
