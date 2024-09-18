#version 120
varying vec3 vCol;
void main()
{
	gl_FragColor = vec4(vCol, 1.0);
}