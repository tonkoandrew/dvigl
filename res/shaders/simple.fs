#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform sampler2D texture;

varying vec2 v_texcoord;
varying vec3 v_normal;
varying vec4 pos;


void main()
{
	gl_FragColor = texture2D(texture, v_texcoord);
    // gl_FragColor = vec4((1.0-v_normal)*0.5, 1.0);
}
