#version 330 core
in vec4 vColor;
in vec2 vTextureCoords;

in vec3 vertexPosi;

out vec4 color;
uniform sampler2D uTexture;

uniform vec4 uColor;
uniform float custom_depth = 0.0;

void main()
{
    gl_FragDepth = custom_depth;
    float value = texture(uTexture,vTextureCoords).x;
    color = vec4(value,value,value,value) * uColor;
    //color = uColor;
}