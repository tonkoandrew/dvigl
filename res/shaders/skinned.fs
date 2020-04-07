#version 410 core

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif


in vec2 v_texcoord;
in vec4 v_pos;
in mat3 TBN;


uniform sampler2D normalMap;
uniform sampler2D albedoMap;

// out vec4 FragColor;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

void main()
{
    vec4 color = texture(albedoMap, v_texcoord);
    gPosition = v_pos.xyz;
    vec3 normal = texture(normalMap, v_texcoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    gNormal = normalize(TBN * normal); 

    gAlbedoSpec.rgb = color.rgb;
    gAlbedoSpec.a = 0.5;
}
