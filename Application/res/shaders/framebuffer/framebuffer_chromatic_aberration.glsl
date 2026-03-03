#version 330 core
in vec4 vColor;
in vec2 vTextureCoords;

in vec3 vertexPosi;

out vec4 color;
uniform sampler2D uTexture;
uniform float uSize = 0.01;

void main()
{
    color.r = texture(uTexture,vec2(vTextureCoords.x + uSize, vTextureCoords.y)).r;
    color.g = texture(uTexture,vTextureCoords).g;
    color.b = texture(uTexture,vec2(vTextureCoords.x - uSize, vTextureCoords.y)).b;
    color.a = 1.0;

    color *= (1.0 - (uSize / 2));
}