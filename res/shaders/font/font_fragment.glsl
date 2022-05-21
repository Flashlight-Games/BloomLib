#version 330 core
in vec4 vColor;
in vec2 vTextureCoords;

in vec3 vertexPosi;

out vec4 color;
uniform sampler2D uTexture;

uniform vec4 uColor;

void main()
{
    color = vec4(1,1,1,texture(uTexture,vTextureCoords).x) * uColor;
    //color = uColor;
}