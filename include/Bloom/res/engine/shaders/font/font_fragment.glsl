#version 330 core
in vec4 vColor;
in vec2 vTextureCoords;
in vec3 vertexPosi;

out vec4 color;
uniform sampler2D uTexture;
uniform vec4 uColor;
uniform float custom_depth = 0.0;

void main()
{
    gl_FragDepth = custom_depth;
    vec2 texOffset = 1.0 / textureSize(uTexture, 0); // gets size of single texel
    vec4 result = vec4(0.0);

    // averaging the surrounding pixels
    result += texture(uTexture, vTextureCoords + texOffset * vec2(-1, -1));
    result += texture(uTexture, vTextureCoords + texOffset * vec2( 0, -1));
    result += texture(uTexture, vTextureCoords + texOffset * vec2( 1, -1));
    result += texture(uTexture, vTextureCoords + texOffset * vec2(-1,  0));
    result += texture(uTexture, vTextureCoords + texOffset * vec2( 0,  0));
    result += texture(uTexture, vTextureCoords + texOffset * vec2( 1,  0));
    result += texture(uTexture, vTextureCoords + texOffset * vec2(-1,  1));
    result += texture(uTexture, vTextureCoords + texOffset * vec2( 0,  1));
    result += texture(uTexture, vTextureCoords + texOffset * vec2( 1,  1));
    result /= 9.0; // average the sample results

    float value = result.x; // use the averaged value

    if(value <= 0.3)
        discard;

    color = vec4(value,value,value,value) * uColor;
}