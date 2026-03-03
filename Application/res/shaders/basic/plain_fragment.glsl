#version 330 core
out vec4 color;

uniform float custom_depth = 0.0;

void main()
{
    gl_FragDepth = custom_depth;
    color = vec4(1.0,0.0,0.0,1.0);
}