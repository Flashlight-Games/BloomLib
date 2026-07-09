#version 330 core
in vec4 vColor;
in vec2 vTextureCoords;

out vec4 color;

uniform vec4 uColor = vec4(1,1,1,1);
uniform sampler2D uTexture;
uniform float custom_depth = 0.0;

void main()
{
    gl_FragDepth = custom_depth;
    vec4 texColor = texture(uTexture,vTextureCoords);
    color = vec4(texColor) * uColor * 10;
    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(color.a == 0)
        discard;
    if(brightness <= 0.0)
        color = vec4(0.01,0.01,0.01,1);
}