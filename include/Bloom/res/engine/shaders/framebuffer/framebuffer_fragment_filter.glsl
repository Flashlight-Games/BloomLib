#version 330 core
in vec4 vColor;
in vec2 vTextureCoords;

in vec3 vertexPosi;

out vec4 color;
uniform sampler2D uTexture;
uniform float intensity = 1.0;

void main()
{
    color = texture(uTexture,vTextureCoords) * intensity;
}