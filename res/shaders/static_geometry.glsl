#shader-type vertex
#version 410 core

layout(location = 0) in vec4 attr_pos;
layout(location = 1) in vec3 attr_normal;
layout(location = 2) in vec3 attr_tangent;
layout(location = 3) in vec2 attr_texcoord;

uniform mat3 normal_matrix;
uniform mat4 mvp;
uniform mat4 model;

out vec2 v_texcoord;
out vec4 v_pos;
out mat3 TBN;

void main()
{
    v_texcoord = attr_texcoord;
    v_pos = model * attr_pos;


    vec3 T = normalize(normal_matrix * attr_tangent);
    vec3 N = normalize(normal_matrix * attr_normal);
    vec3 B = normalize(cross(T, N));
    TBN = mat3(T, B, N);

    gl_Position = mvp * attr_pos;
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
in vec4 v_pos;
in mat3 TBN;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec4 out_albedoSpec;

void main()
{
    vec4 color = texture(albedoMap, v_texcoord);
    out_position = v_pos.xyz;

    vec3 BumpMapNormal = texture(normalMap, v_texcoord).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    out_normal = normalize(TBN * BumpMapNormal);

    out_albedoSpec.rgb = color.rgb;
    out_albedoSpec.a = 0.5;
}
