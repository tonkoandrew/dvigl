#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform sampler2D texture;

varying vec3 v_normal;
varying vec2 v_texcoord;

varying vec4 pos;
uniform int is_pressed;
uniform int is_hover;

void main()
{
	vec4 diffuse = texture2D(texture, v_texcoord);
	diffuse.a = 1.0;
    // gl_FragColor = vec4(diffuse.xyz * 1.1, diffuse.a);
	gl_FragColor = diffuse;
    // gl_FragColor = vec4((1.0-v_normal)*0.5, 1.0);
}
