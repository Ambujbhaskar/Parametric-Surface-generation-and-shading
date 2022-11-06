#version 330 core

varying vec3 N;
in vec4 fragColor;
out vec4 FragColor;

void main()

{	

    vec3 nColor = vec3((N.x+1)/2, (N.y+1)/2, (N.z+1)/2);
	FragColor = vec4(vec3(fragColor) * nColor, 1.0);
}
