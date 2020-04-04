#version 410 core

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform sampler2D tex;

in vec2 v_texcoord;
in vec3 v_normal;
in vec4 v_pos;

out vec4 FragColor;

void main()
{
	vec4 color = texture(tex, v_texcoord);
	if(color.a < 0.1)
		discard;
	FragColor = color;
    // FragColor = vec4((1.0-v_normal)*0.5, 1.0);
}
