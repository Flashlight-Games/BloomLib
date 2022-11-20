#version 330 core
in vec4 vColor;
in vec2 vTextureCoords;

in vec3 vertexPosi;

out vec4 color;
uniform sampler2D uTexture;
uniform float intensity = 1.0;

void main()
{
    color = texture(uTexture,vTextureCoords) * intensity;
    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness <= 0.0){
        discard;
    }
   
}