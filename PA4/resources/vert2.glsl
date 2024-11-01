#version 120

attribute vec3 vPositionModel;
attribute vec3 vNormalModel;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec3 fragPosition;  // Position in world space
varying vec3 fragNormal;    // Normal in world space

void main()
{
    vec4 worldPos = model * vec4(vPositionModel, 1.0);
    fragPosition = worldPos.xyz;
    fragNormal = vec3(model * vec4(vNormalModel,1.0));

	gl_Position = projection * view * model * vec4(vPositionModel, 1.0);
}
