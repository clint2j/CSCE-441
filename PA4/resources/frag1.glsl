#version 120

varying vec3 fragPosition;  // Position in world space
varying vec3 fragNormal;    // Normal in world space

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

struct lightStruct {
    vec3 position;
    vec3 color;
};

#define NUM_LIGHTS 2

uniform lightStruct lights[NUM_LIGHTS];
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;

void main()
{
	vec3 N = fragNormal;
	vec3 V = normalize(-vec3(view * vec4(fragPosition,1.0f)));
	vec3 I = ka;

	for (int i = 0; i< NUM_LIGHTS; ++i){
		vec3 L = normalize(lights[i].position - fragPosition);
		vec3 R = reflect(-L,N);
		float diffuse = max(0.0, dot(L,N));
		float specular = pow(max(0.0,dot(R,V)),s);
		I += lights[i].color * (kd * diffuse + ks * specular);
	}
	gl_FragColor = vec4(I,1.0f);
	// vec3 normalizedPos = normalize(fragPosition);
    // gl_FragColor = vec4(normalizedPos * 0.5 + 0.5, 1.0);
}
