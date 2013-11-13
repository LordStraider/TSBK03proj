#version 330 core

in vec2 frag_TexCoord;

out vec4 out_Color;

uniform sampler2D texUnit1;
uniform sampler2D texUnit2;

void main(void) {
	out_Color =  vec4(texture(texUnit1, frag_TexCoord).rgb + texture(texUnit2, frag_TexCoord).rgb, 1.0);
}
