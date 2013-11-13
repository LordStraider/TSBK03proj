#version 330 core

in vec2 frag_TexCoord;

out vec4 out_Color;

uniform sampler2D texUnit;

void main(void) {
	out_Color =  max(texture(texUnit, frag_TexCoord) - vec4(1.0, 1.0, 1.0, 0.0), 0);
}
