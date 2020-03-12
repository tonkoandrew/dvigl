#version 330

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
