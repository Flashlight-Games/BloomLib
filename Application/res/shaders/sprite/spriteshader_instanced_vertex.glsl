#version 330 core
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 textureCoords;

out vec4 vColor;
out vec2 vTextureCoords;

uniform mat4 model[64];
uniform vec4 color[64];
//uniform vec2 texuv[128];

uniform mat4 view;
uniform mat4 projection;

void main()
{
	int id = gl_InstanceID;
	gl_Position = projection * view * model[id] * vec4(vertexPos.x, vertexPos.y, 0.0, 1.0);
	vTextureCoords = textureCoords;// + texuv[gl_InstanceID];
	vColor = color[id];
}