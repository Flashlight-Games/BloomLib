#version 330 core
in vec4 vColor;
in vec2 vTextureCoords;

in vec3 vertexPosi;

out vec4 color;
uniform sampler2D uTexture;
uniform int uSize = 5;

void main()
{
    vec2 tex_offset = 1.0 / textureSize(uTexture, 0); // gets size of single texel

    vec4 result = texture(uTexture, vTextureCoords); // current fragment's contribution
    if(uSize > 0){
        for(int i = 1; i < uSize; ++i)
        {
            result += texture(uTexture, vTextureCoords + vec2(tex_offset.x * i,0));
            result += texture(uTexture, vTextureCoords - vec2(tex_offset.x * i,0));
        
        }
        result = result / uSize;
        result *= 0.9;
    }
    color = result;
}