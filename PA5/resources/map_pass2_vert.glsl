#version 120

attribute vec3 vPositionModel;
attribute vec3 vNormalModel;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightViewProjection;

varying vec3 fragPosition;
varying vec3 fragNormal;
varying float fragDepth;  // Add this varying variable
varying vec4  lightSpacePos;


void main()
{
    fragPosition = vPositionModel;
    fragNormal = vNormalModel;
    vec4 position = projection * view * model * vec4(vPositionModel, 1.0);
    gl_Position = position;
    lightSpacePos = lightViewProjection * model * vec4(vPositionModel, 1.0);
}
