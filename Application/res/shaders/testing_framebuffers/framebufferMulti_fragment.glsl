#version 330 core
in vec4 vColor;
in vec2 vTextureCoords;

in vec3 vertexPosi;

out vec4 color;
uniform sampler2D uTexture;
uniform sampler2D uTextureTwo;
uniform float custom_depth = 0.0;

void main()
{
    gl_FragDepth = custom_depth;
    color = mix(texture(uTexture,vTextureCoords),texture(uTextureTwo,vTextureCoords),0.5);
}