#version 330 core
in vec4 vColor;
in vec2 vTextureCoords;

out vec4 color;

uniform vec4 uColor = vec4(1,1,1,1);
uniform sampler2D uTexture;

void main()
{
    vec4 texColor = texture(uTexture,vTextureCoords);
    color = vec4(texColor) * uColor;
}