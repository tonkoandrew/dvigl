#version 330

in vec2 TexCoord0;

struct VSOutput
{
    vec2 TexCoord;                                                            
};

uniform sampler2D gColorMap;        
uniform sampler2D gNormalMap;

out vec4 FragColor;
                                                                
void main()
{                                    
    VSOutput In;
    In.TexCoord = TexCoord0;

    // FragColor = texture(gColorMap, In.TexCoord.xy);
    // FragColor = texture(gNormalMap, In.TexCoord.xy) * 0.0005 + texture(gColorMap, In.TexCoord.xy)*0.9995;
    FragColor = texture(gNormalMap, In.TexCoord.xy) * 0.005 + texture(gColorMap, In.TexCoord.xy)*0.995 + vec4(0.0);

}
