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


out vec4 v_pos_cam;
// out vec4 v_prev_pos_cam;


uniform mat3 normalMatrix;
uniform mat4 model;
uniform mat4 mvp;
// uniform mat4 prev_mvp;


void main()
{
    vec3 T = normalize(normalMatrix * attr_tangent);
    vec3 B = normalize(normalMatrix * attr_bitangent);
    vec3 N = normalize(normalMatrix * attr_normal);
    TBN = mat3(T, B, N);

    v_texcoord = attr_texcoord;

    v_pos = (model * vec4(attr_pos, 1.0)).xyz;

    v_pos_cam = mvp * vec4(attr_pos, 1.0);
    // v_prev_pos_cam = prev_mvp * vec4(attr_pos, 1.0);

    gl_Position = v_pos_cam;
}

#defshader fragment
#version 410 core

layout(location = 0) out vec4 gb_Albedo;
layout(location = 1) out vec3 gb_Normal;
layout(location = 2) out vec4 gb_MaterialInfo;
layout(location = 3) out vec3 gb_WorldPos;
layout(location = 4) out vec3 gb_Velocity;

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

in vec4 v_pos_cam;
// in vec4 v_prev_pos_cam;

uniform Material material;
uniform float time_delta;

// Unpacks the normal from the texture and returns the normal in tangent space
vec3 UnpackNormal(vec3 textureNormal) {
    return normalize(textureNormal * 2.0 - 1.0);
}

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

    // vec2 a = (v_pos_cam.xy / v_pos_cam.w) * 0.5 + 0.5;
    // vec2 b = (v_prev_pos_cam.xy / v_prev_pos_cam.w) * 0.5 + 0.5;
    // vec2 vel = (a - b);
    // gb_Velocity = vec3(vel, 1.0);
    gb_Velocity = vec3(1.0);
}
