#version 120

attribute vec3 vPositionModel;
attribute vec3 vNormalModel;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec3 fragPosition;
varying vec3 fragNormal;

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
    fragPosition = vPositionModel;
    fragNormal = vNormalModel;
	gl_Position = projection * view * model * vec4(vPositionModel, 1.0);
}
