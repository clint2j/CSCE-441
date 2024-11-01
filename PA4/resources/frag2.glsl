#version 120

varying vec3 fragPosition;  // Position in world space
varying vec3 fragNormal;    // Normal in world space

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	float threshold = 0.4f;
	vec3 normalizedPos = normalize(fragPosition);
	vec3 eye = -inverse(mat3(view)) * vec3(view[3]);
    gl_FragColor = vec4(normalizedPos * 0.5 + 0.5, 1.0);
	// float EE = dot(fragNormal,)
	// if ()
}
