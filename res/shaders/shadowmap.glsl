#defshader vertex
#version 410 core
layout(location = 0) in vec3 attr_pos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(attr_pos, 1.0);
}

#defshader fragment
#version 410 core
void main()
{             
    // gl_FragDepth = gl_FragCoord.z;
}