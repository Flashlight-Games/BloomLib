#version 330 core
in vec4 vColor;
in vec2 vTextureCoords;

in vec3 vertexPosi;

out vec4 color;
uniform sampler2D uTexture;

void main()
{
    color = texture(uTexture,vTextureCoords);
    if(color.a <= 0.0)
        //discard;
        color = vec4(0.21,0.21,0.21,1);
    color = vec4(color.xyz,1.0);
    
}