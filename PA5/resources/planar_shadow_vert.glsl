#version 120

attribute vec3 vPositionModel;
attribute vec3 vNormalModel;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
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
    vec3 lightToGround  = p_ground - lights[0].position;
    float num = dot(lightToGround ,n_ground);
    vec4 vertexWorldSpace  = model * vec4(vPositionModel, 1.0);
    vec3 lightToVertexDir  = normalize(vertexWorldSpace .xyz-lights[0].position);
    float denom = dot(lightToVertexDir,n_ground);
    if (abs(denom) > 0.0001){
        float t = num/denom;
        vec3 intersectionPoint = lights[0].position+.9999*t*lightToVertexDir;
        gl_Position = projection * view * vec4(intersectionPoint,1.0);
    }
    else {
        gl_Position = vec4(2,2,2,1.0);
    }
}