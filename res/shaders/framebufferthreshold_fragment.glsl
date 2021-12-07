#version 330 core
in vec4 vColor;
in vec2 vTextureCoords;

in vec3 vertexPosi;

out vec4 color;
uniform sampler2D uTexture;

void main()
{
    color = vec4(vec3((texture(uTexture,vTextureCoords).x + texture(uTexture,vTextureCoords).y + texture(uTexture,vTextureCoords).z) / 3 > 0.225),1.0);
}