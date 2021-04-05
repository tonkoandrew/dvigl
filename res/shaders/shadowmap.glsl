#defshader vertex
#version 410 core
layout(location = 0) in vec3 attr_pos;

uniform mat4 light_mvp;

void main()
{
    gl_Position = light_mvp * vec4(attr_pos, 1.0);
}

#defshader fragment
#version 410 core
void main()
{             
    // gl_FragDepth = gl_FragCoord.z;
}