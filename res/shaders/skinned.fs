#version 410 core

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif


in vec2 TexCoord0;

struct VSOutput
{
    vec2 TexCoord;                                                            
};

uniform sampler2D gColorMap;        

// out vec4 FragColor;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

void main()
{                                    
    VSOutput In;
    In.TexCoord = TexCoord0;
    // FragColor = texture(gColorMap, In.TexCoord.xy);
    gAlbedoSpec = texture(gColorMap, In.TexCoord.xy);
}
