#version 330 core
in vec4 vColor;
in vec2 vTextureCoords;

in vec3 vertexPosi;

out vec4 color;
uniform sampler2D uTexture;

uniform vec3 data;
uniform vec2 position;
int thickness = 1;
uniform float time;

void main()
{
    color = vec4(0,0,0,1);
    float cdist = distance(gl_FragCoord.xy,data.xy/2);
    if(cdist < data.z+thickness && cdist > data.z-0)
        color = vec4(0.15,0.15,0.15,(1-distance(gl_FragCoord.xy,position)/30)>0.5 ? 1 : 0.2);
    else
        color = vec4(texture(uTexture,vTextureCoords).xyz,0);
}