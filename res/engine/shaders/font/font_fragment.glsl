#version 330 core
in vec4 vColor;
in vec2 vTextureCoords;

in vec3 vertexPosi;

out vec4 color;
uniform sampler2D uTexture;

uniform vec4 uColor;

void main()
{
    float value = texture(uTexture,vTextureCoords).x;
    color = vec4(value,value,value,value) * uColor;
    //color = uColor;
}