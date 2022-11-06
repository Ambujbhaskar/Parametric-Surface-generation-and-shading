#version 330 core
layout (location = 0)in vec3 vVertex;
layout (location = 1)in vec3 vertex_norm;

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;
uniform vec3 vColor;

varying vec3 N;

out vec4 fragColor;


void main(){

    gl_Position = vProjection * vView * vModel * vec4(vVertex, 1.0);

    N = normalize(vec3(vec4(vertex_norm, 1.0) * inverse(vModel)));

    fragColor = vec4(vColor, 1.0f);
}
