#version 330 core
in vec4 vColor;
in vec2 vTextureCoords;

out vec4 color;

uniform float uColor = 0;
uniform sampler2D uTexture;

void main()
{
    float light_ambient = 1.0;
    vec4 texColor = texture(uTexture,vTextureCoords) * abs(uColor);
    color = vec4(texColor);
}