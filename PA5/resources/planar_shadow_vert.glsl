#version 120

attribute vec3 vPositionModel;
attribute vec3 vNormalModel;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 eye;
uniform vec3 n_ground;
uniform vec3 p_ground;

struct lightStruct {
    vec3 position;
    vec3 color;
};

#define NUM_LIGHTS 1

uniform lightStruct lights[NUM_LIGHTS];

varying vec3 color;

void main() {
    color = vec3(0,0,0);
    vec3 p1 = p_ground - lights[0].position;
    float num = dot(p1,n_ground);
    vec4 pt = projection * view * model * vec4(vPositionModel, 1.0);
    float scaleFactor = 1.0f/pt.w;
    vec3 pt2 = vec3(scaleFactor * pt);
    vec3 d = normalize(pt2-lights[0].position);
    float denom = dot(d,n_ground);
    float t = num/denom;
    vec3 finalPt = lights[0].position+.999*t*d;
    gl_Position = vec4(finalPt,1.0f);
}