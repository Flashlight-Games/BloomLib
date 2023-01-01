#version 330 core
in vec4 vColor;
in vec2 vTextureCoords;

in vec3 vertexPosi;

out vec4 color;
uniform sampler2D uTexture;
uniform sampler2D uTextureTwo;

void main()
{
    color = mix(texture(uTexture,vTextureCoords),texture(uTextureTwo,vTextureCoords),0.5);
}