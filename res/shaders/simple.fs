#version 330

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
	FragColor = texture(tex, v_texcoord);
    // FragColor = vec4((1.0-v_normal)*0.5, 1.0);
}
