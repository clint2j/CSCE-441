#version 120

varying vec3 fragPosition;
varying vec3 fragNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 eye;

void main()
{
    vec3 viewDir = normalize(eye - fragPosition);
    float NdotV = abs(dot(normalize(fragNormal), normalize(eye)));

	float threshold = 0.33f;
	if (NdotV<threshold){
		gl_FragColor = vec4(1.0,1.0,1.0, 1.0);
	}
	else {
		gl_FragColor = vec4(0);
	}
}
