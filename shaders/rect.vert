#version 330 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec2 in_TexCoord;

out vec2 frag_TexCoord;

void main(void) {
	frag_TexCoord = in_TexCoord;
	gl_Position = vec4(in_Position, 1);
}