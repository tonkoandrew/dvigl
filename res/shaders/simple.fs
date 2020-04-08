#version 410 core

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform sampler2D normalMap;
uniform sampler2D albedoMap;

in vec2 v_texcoord;
in vec3 v_normal;
in vec3 v_tangent;
in vec4 v_pos;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec4 out_albedoSpec;

void main()
{
    vec4 color = texture(albedoMap, v_texcoord);
    out_position = v_pos.xyz;

    vec3 Normal = normalize(v_normal);
    vec3 Tangent = normalize(v_tangent);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Tangent, Normal);
    vec3 BumpMapNormal = texture(normalMap, v_texcoord).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    out_normal = normalize(TBN * BumpMapNormal);

    out_albedoSpec.rgb = color.rgb;
    out_albedoSpec.a = 1.0;
}
