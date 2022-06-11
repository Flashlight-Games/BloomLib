#version 330 core
in vec4 vColor;
in vec2 vTextureCoords;

in vec3 vertexPosi;

out vec4 color;
uniform sampler2D uTexture;

uniform vec2 uCenter;
int thickness = 1;
uniform float uRadius = 0;

void main()
{
    float cdist = distance(gl_FragCoord.xy,uCenter.xy);
    if(cdist <uRadius+thickness && cdist > uRadius-0)
        color = vec4(0.0, 0.6, 1, 1);
    else
        color = texture(uTexture,vTextureCoords);

    if(cdist < uRadius)
        discard;
}