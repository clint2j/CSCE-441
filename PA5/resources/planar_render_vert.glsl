#version 120

attribute vec3 vPositionModel;
attribute vec3 vNormalModel;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

struct lightStruct {
    vec3 position;
    vec3 color;
};

#define NUM_LIGHTS 1

uniform lightStruct lights[NUM_LIGHTS];
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;

varying vec3 color;

void main() {
    vec4 worldPos = model * vec4(vPositionModel, 1.0);
    vec3 N = normalize(mat3(model) * vNormalModel);
    vec3 V = normalize(-vec3(view * worldPos));
    vec3 I = ka;

    for (int i = 0; i < NUM_LIGHTS; ++i) {
        vec3 L = normalize(lights[i].position - vec3(worldPos));
        vec3 R = reflect(-L, N);
        float diffuse = max(0.0, dot(L, N));
        float specular = pow(max(0.0, dot(R, V)), s);
        I += lights[i].color * (kd * diffuse + ks * specular);
    }

    color = I;
    gl_Position = projection * view * model * vec4(vPositionModel, 1.0);
}