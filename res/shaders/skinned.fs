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

out vec4 FragColor;
                                                                
void main()
{                                    
    VSOutput In;
    In.TexCoord = TexCoord0;
    FragColor = texture(gColorMap, In.TexCoord.xy);
}
