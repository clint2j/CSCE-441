#version 120
uniform mat4 Proj;
uniform mat4 View;
attribute vec3 Pos;
attribute vec3 Col;
varying vec3 vCol;
void main()
{
	vCol = Col;
	gl_Position = Proj * (View * vec4(Pos, 1.0));
}