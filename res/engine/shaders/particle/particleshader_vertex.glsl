#version 330 core
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexColor;
layout (location = 2) in vec2 textureCoords;

out vec4 vColor;
out vec2 vTextureCoords;

uniform mat4 model[128];
uniform vec4 color[128];

uniform mat4 view;
uniform mat4 projection;

void main()
{
   gl_Position = projection * view * model[gl_InstanceID] * vec4(vertexPos.x,vertexPos.y,0.0, 1.0);
   vColor = color[gl_InstanceID];
   vTextureCoords = textureCoords;
}