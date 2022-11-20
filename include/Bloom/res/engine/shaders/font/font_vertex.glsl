#version 330 core
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 textureCoords;

out vec2 vTextureCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
   gl_Position =  model * vec4(vertexPos.x,vertexPos.y,0.0, 1.0);
   vTextureCoords = textureCoords;
}