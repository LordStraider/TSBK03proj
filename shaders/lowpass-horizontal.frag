#version 330 core

in vec2 frag_TexCoord;

out vec4 out_Color;

uniform sampler2D texUnit;

const float coeff[5] = float[5](1, 1, 1, 1, 1);

vec2 coord_offset(vec2 c, vec2 offset) {
	return c + offset/textureSize(texUnit, 0);
}

void main(void) {

	vec3 value = vec3(0, 0, 0);

	for (int i = -2; i <= 2; ++i) {
		value += coeff[i+2]*texture(texUnit, coord_offset(frag_TexCoord, vec2(i, 0))).rgb;
	}

	out_Color = vec4(value, 1) / 4.98;
}
