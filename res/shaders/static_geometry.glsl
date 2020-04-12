#shader-type vertex
#version 410 core

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
// precision mediump vec2;
#endif

layout(location = 0) in vec3 attr_pos;
layout(location = 1) in vec3 attr_normal;
layout(location = 2) in vec2 attr_texcoord;
layout(location = 3) in vec3 attr_tangent;
layout(location = 4) in vec3 attr_bitangent;

out vec2 v_texcoord;
out vec4 v_pos;

out vec3 v_tangent;
out vec3 v_bitangent;
out vec3 v_normal;

uniform mat4 mvp;
uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;

void main()
{
    v_texcoord = attr_texcoord;
    v_pos = model * vec4(attr_pos, 1.0);
    v_normal = (mvp * vec4(attr_normal, 0.0)).xyz;
    v_tangent = (mvp * vec4(attr_tangent, 0.0)).xyz;
    v_bitangent = (mvp * vec4(attr_bitangent, 0.0)).xyz;

    gl_Position = mvp * vec4(attr_pos, 1.0);
}


#shader-type fragment
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
in vec3 v_bitangent;
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
    vec3 Bitangent = normalize(v_bitangent);
    vec3 BumpMapNormal = texture(normalMap, v_texcoord).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    out_normal = normalize(TBN * normalize(BumpMapNormal));

    out_albedoSpec.rgb = color.rgb;
    out_albedoSpec.a = 1.0;
}
