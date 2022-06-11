#version 330 core
in vec4 vColor;
in vec2 vTextureCoords;

in vec3 vertexPosi;

out vec4 color;
uniform sampler2D uTexture;

uniform vec3 uData;
uniform vec2 uPosition;
int thickness = 1;
uniform float uTime;

void main()
{
    color = vec4(0,0,0,1);
    float cdist = distance(gl_FragCoord.xy,uData.xy/2);
    if(cdist < uData.z+thickness && cdist > uData.z-0)
        if((1-distance(gl_FragCoord.xy,uPosition)/30)>0.5)
            color = vec4(0.15,0.15,0.15,1);
        else
            color = vec4(0.19,0.19,0.19,1);
    else
        color = vec4(0.21,0.21,0.21,1);
}