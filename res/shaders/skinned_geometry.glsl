#defshader vertex
#version 410 core

layout(location = 0) in vec3 attr_pos;
layout(location = 1) in vec3 attr_normal;
layout(location = 2) in vec2 attr_texcoord;
layout(location = 3) in vec3 attr_tangent;
layout(location = 4) in vec3 attr_bitangent;
layout(location = 5) in ivec4 BoneIDs;
layout(location = 6) in vec4 Weights;

out mat3 TBN;
out vec2 v_texcoord;
out vec3 v_pos;

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
    v_pos = (model * (BoneTransform * vec4(attr_pos, 1.0))).xyz;

    vec4 norm = BoneTransform * vec4(attr_normal, 0.0);
    vec4 tang = BoneTransform *vec4(attr_tangent, 0.0);
    // vec4 bitang = vec4(cross(norm.xyz, tang.xyz), 0.0);
    vec4 bitang = BoneTransform *vec4(attr_bitangent, 0.0);

    vec3 T = normalize(normalMatrix * tang.xyz);
    vec3 B = normalize(normalMatrix * bitang.xyz);
    vec3 N = normalize(normalMatrix * norm.xyz);
    TBN = mat3(T, B, N);

    gl_Position = mvpb * vec4(attr_pos, 1.0);
}

#defshader fragment
#version 410 core

layout(location = 0) out vec4 gb_Albedo;
layout(location = 1) out vec3 gb_Normal;
layout(location = 2) out vec4 gb_MaterialInfo;
layout(location = 3) out vec3 gb_WorldPos;

struct Material
{
    sampler2D texture_albedo;
    sampler2D texture_normal;
    sampler2D texture_metallic;
    sampler2D texture_roughness;
    sampler2D texture_ao;
};

in mat3 TBN;
in vec2 v_texcoord;
in vec3 v_pos;

uniform Material material;

vec3 UnpackNormal(vec3 textureNormal);

void main()
{
    vec2 textureCoordinates = v_texcoord;
    vec4 albedo = texture(material.texture_albedo, textureCoordinates);
    vec3 normal = texture(material.texture_normal, textureCoordinates).rgb;
    float metallic = texture(material.texture_metallic, textureCoordinates).r;
    float roughness = max(texture(material.texture_roughness, textureCoordinates).r, 0.04);
    float ao = texture(material.texture_ao, textureCoordinates).r;

    gb_Albedo = albedo;
    gb_Normal = normalize(TBN * UnpackNormal(normal));

    gb_MaterialInfo = vec4(metallic, roughness, ao, 1.0);

    gb_WorldPos = v_pos;
}

// Unpacks the normal from the texture and returns the normal in tangent space
vec3 UnpackNormal(vec3 textureNormal) {
    return normalize(textureNormal * 2.0 - 1.0);
}