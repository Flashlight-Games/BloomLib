#version 330 core
in vec4 vColor;
in vec2 vTextureCoords;

out vec4 color;

uniform sampler2D uTexture;
uniform float custom_depth = 0.0;

void main()
{
    gl_FragDepth = custom_depth;
    vec4 texColor = texture(uTexture,vTextureCoords);
    color = vec4(texColor) * vColor;
}