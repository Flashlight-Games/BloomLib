#version 330 core
in vec2 vTextureCoords;

out vec4 color;
uniform sampler2D uTexture;

uniform int size = 32;
uniform int strength = 1;

vec4 brightFilter(vec4 inColor,float thresh){
    if((inColor.x>= thresh ||  inColor.y >= thresh || inColor.z>= thresh) && inColor.a == 1)
        return inColor;
    return vec4(0,0,0,0);
}

void main()
{    
    vec2 tex_offset = 1.0 / textureSize(uTexture, 0); // gets size of single texel
    vec4 sum = vec4(0,0,0,0);
    vec4 offset_color;

    if(strength > 0 && size > 0)    
        for(int i = -size; i <= size; i++){
            offset_color = brightFilter(texture(uTexture,vTextureCoords + vec2(tex_offset.x * i,0)),0.95);
            sum+= vec4(offset_color.rgb * 1 / (size) + vec3(0.05,0.05,0.05) / size,offset_color.a * 1 / (size) * 0.9 * strength);
        }
    if(sum.a / strength < 1.0)
        color = sum;
    else{
        color = texture(uTexture,vTextureCoords);
    }
}