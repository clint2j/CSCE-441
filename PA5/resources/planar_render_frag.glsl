#version 120

varying vec3 color;

void main()
{
	gl_FragColor = vec4(color, 1.0f);
	//gl_FragColor = vec4(1,1,1, 1.0f);
}
