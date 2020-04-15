#defshader vertex
#version 410 core

layout(location = 0) in vec3 attr_pos;
layout(location = 1) in vec3 attr_normal;
layout(location = 2) in vec2 attr_texcoord;
layout(location = 3) in vec3 attr_tangent;
layout(location = 4) in vec3 attr_bitangent;



out mat3 TBN;
out vec2 v_texcoord;
out vec3 v_pos;

uniform vec3 viewPos;

uniform mat3 normalMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec3 T = normalize(normalMatrix * attr_tangent);
    vec3 B = normalize(normalMatrix * attr_bitangent);
    vec3 N = normalize(normalMatrix * attr_normal);
    TBN = mat3(T, B, N);

    v_texcoord = attr_texcoord;

    vec3 fragPos = vec3(model * vec4(attr_pos, 1.0f));
    gl_Position = projection * view * vec4(fragPos, 1.0);

    v_pos = (model * vec4(attr_pos, 1.0)).xyz;
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
