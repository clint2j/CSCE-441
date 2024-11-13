#version 120

varying vec3 fragPosition;
varying vec3 fragNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

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
    // Transform and normalize the normal in fragment shader
	vec4 worldPos = model * vec4(fragPosition, 1.0);
    vec3 N = normalize(mat3(model) * fragNormal);
    vec3 V = normalize(-vec3(view * worldPos));
    vec3 I = ka;
    
    for (int i = 0; i < NUM_LIGHTS; ++i) {
        vec3 L = normalize(lights[i].position - vec3(worldPos));
        vec3 R = reflect(-L, N);
        float diffuse = max(0.0, dot(L,N));
        float specular = pow(max(0.0, dot(R,V)), s);
        I += lights[i].color * (kd * diffuse + ks * specular);
    }
    gl_FragColor = vec4(I,1.0);
}