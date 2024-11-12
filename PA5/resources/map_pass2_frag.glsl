#version 120


#define NUM_LIGHTS 1

struct lightStruct
{
	vec3 position;
	vec3 color;
};

uniform lightStruct lights[NUM_LIGHTS];

uniform sampler2D shadowMap;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;


void main()
{
	gl_FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}