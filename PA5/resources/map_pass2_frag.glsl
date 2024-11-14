#version 120

varying vec3 fragPosition;
varying vec3 fragNormal;
varying vec4 lightSpacePos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;
uniform sampler2D shadowMap;

struct lightStruct {
    vec3 position;
    vec3 color;
};

#define NUM_LIGHTS 2
uniform lightStruct lights[NUM_LIGHTS];

void main() {
    vec4 worldPos = model * vec4(fragPosition, 1.0);
    vec3 N = normalize(mat3(model) * fragNormal);
    vec3 V = normalize(-vec3(view * worldPos));
    vec3 I = ka;

    vec3 projCoords = (lightSpacePos.xyz / lightSpacePos.w) * 0.5 + 0.5;
    float shadowDepth = 100.0;
    
    if(projCoords.x >= -1 && projCoords.x <= 1.0 && 
       projCoords.y >= -1 && projCoords.y <= 1.0) {
        shadowDepth = texture2D(shadowMap, projCoords.xy).r;
    }

    if (projCoords.z - 0.001 <= shadowDepth) {
        for (int i = 0; i < NUM_LIGHTS; ++i) {
            vec3 L = normalize(lights[i].position - vec3(worldPos));
            vec3 R = reflect(-L, N);
            float diffuse = max(0.0, dot(L, N));
            float specular = pow(max(0.0, dot(R, V)), s);
            I += lights[i].color * (kd * diffuse + ks * specular);
        }
    }
    
    gl_FragColor = vec4(I, 1.0);
}