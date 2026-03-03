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

    float x = tex_offset.x;
    float y = tex_offset.y;

    vec4 a = texture(uTexture, vec2(vTextureCoords.x - 2*x, vTextureCoords.y + 2*y));
    vec4 b = texture(uTexture, vec2(vTextureCoords.x,       vTextureCoords.y + 2*y));
    vec4 c = texture(uTexture, vec2(vTextureCoords.x + 2*x, vTextureCoords.y + 2*y));
       
    vec4 d = texture(uTexture, vec2(vTextureCoords.x - 2*x, vTextureCoords.y));
    vec4 e = texture(uTexture, vec2(vTextureCoords.x,       vTextureCoords.y));
    vec4 f = texture(uTexture, vec2(vTextureCoords.x + 2*x, vTextureCoords.y));
       
    vec4 g = texture(uTexture, vec2(vTextureCoords.x - 2*x, vTextureCoords.y - 2*y));
    vec4 h = texture(uTexture, vec2(vTextureCoords.x,       vTextureCoords.y - 2*y));
    vec4 i = texture(uTexture, vec2(vTextureCoords.x + 2*x, vTextureCoords.y - 2*y));
       
    vec4 j = texture(uTexture, vec2(vTextureCoords.x - x, vTextureCoords.y + y));
    vec4 k = texture(uTexture, vec2(vTextureCoords.x + x, vTextureCoords.y + y));
    vec4 l = texture(uTexture, vec2(vTextureCoords.x - x, vTextureCoords.y - y));
    vec4 m = texture(uTexture, vec2(vTextureCoords.x + x, vTextureCoords.y - y));

    color = vec4(e*0.125);
    color += vec4((a+c+g+i)*0.03125);
    color += vec4((b+d+f+h)*0.0625);
    color += vec4((j+k+l+m)*0.125);
}