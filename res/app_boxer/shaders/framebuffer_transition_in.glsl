#version 330 core
in vec4 vColor;
in vec2 vTextureCoords;

in vec3 vertexPosi;

out vec4 color;
uniform sampler2D uTexture;

uniform vec2 uCenter = vec2(0,0);
int thickness = 1;
uniform float uRadius = 0;
uniform vec3 uColor = vec3(1,0,0);

void main()
{
    float cdist = distance(gl_FragCoord.xy,uCenter.xy);
    if(cdist > uRadius)
        if(texture(uTexture,vTextureCoords).a <= 0)
            color = vec4(uColor, 1);
        else
            color = texture(uTexture,vTextureCoords);
    else
        discard;
}