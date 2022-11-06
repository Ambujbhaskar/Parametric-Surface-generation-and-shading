#version 330 core
layout (location = 0)in vec3 vVertex;
layout (location = 1)in vec3 vertex_norm;

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;
uniform vec3 camera;
uniform vec3 vColor;
uniform vec3 lColor;
uniform vec3 lpos_world;

vec3 Ls = vec3(1.0, 1.0, 1.0);
vec3 Ld = vec3(0.7, 0.7, 0.7);
vec3 La = vec3(0.6, 0.2, 0.2);

vec3 ks = vec3(1.0, 1.0, 1.0);
vec3 kd = vec3(0.5, 0.6, 0.4);
vec3 ka = vec3(1.0, 1.0, 1.0);

float alpha = 32.0;

out vec4 fragColor;


void main(){

    gl_Position = vProjection * vView * vModel * vec4(vVertex, 1.0);


    vec3 VERT = vec3(vModel * vec4(vVertex, 1.0));
    vec3 L = normalize(lpos_world - VERT);
    vec3 N = normalize(vec3(vec4(vertex_norm, 1.0) * inverse(vModel)));
    // vec3 R = normalize(2 * dot(L, N) * N - L);
    vec3 R = normalize(reflect(L, N));
    vec3 V = normalize(vec3(camera));

    vec3 Ia = ka * La;
    vec3 Id = kd * max(dot(L, N) * Ld, 0);
    vec3 Is = ks * Ls * max(pow(dot(R, V), alpha), 0);

    fragColor = vec4(vColor * lColor *(Ia + Id), 1.0f);
}
