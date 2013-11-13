#version 330 core

// Simplified Phong: No materials, only one, hard coded light source
// (in view coordinates) and no ambient

// Note: Simplified! In particular, the light source is given in view
// coordinates, which means that it will follow the camera.
// You usually give light sources in world coordinates.

in vec3 exNormal;
in vec3 exSurface;

out vec4 outColor;

void main(void)
{
  const vec3 color = vec3(0.8, 0.1, 0.1);
	const vec3 light = vec3(0.58, 0.58, 0.58); // Given in VIEW coordinates! You usually specify light sources in world coordinates.
  vec3 shade;
  float diffuse, specular;
	
	// Diffuse
	diffuse = dot(normalize(exNormal), light);
	diffuse = max(0.0, diffuse); // No negative light
	
	// Specular
	vec3 r = reflect(-light, normalize(exNormal));
	vec3 v = normalize(-exSurface); // View direction
	specular = dot(r, v);
	if (specular > 0.0)
		specular = 1.0 * pow(specular, 150.0);
	specular = max(specular, 0.0);
	shade = color*diffuse + color*specular;
	outColor = vec4(shade, 1.0);
}
