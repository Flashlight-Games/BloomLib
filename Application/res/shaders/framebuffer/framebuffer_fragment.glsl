#version 330 core
in vec4 vColor;
in vec2 vTextureCoords;

in vec3 vertexPosi;

out vec4 color;
uniform sampler2D uTexture;
uniform float custom_depth = 0.0;

void main()
{
    gl_FragDepth = custom_depth;
    color = vec4(texture(uTexture,vTextureCoords).xyz,1);
}