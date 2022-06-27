#version 330 core
in vec4 vColor;
in vec2 vTextureCoords;

in vec3 vertexPosi;

out vec4 color;
uniform sampler2D uTexture;
uniform float uSize = 0.01;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 tex_offset = 1.0 / textureSize(uTexture, 0); // gets size of single texel

    vec3 result = texture(uTexture, vTextureCoords).rgb * weight[0]; // current fragment's contribution
    for(int i = 1; i < 5; ++i)
    {
        result += texture(uTexture, vTextureCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        result += texture(uTexture, vTextureCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
    }
    color = vec4(result, 1.0);
}