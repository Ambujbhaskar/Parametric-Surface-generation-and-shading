#version 330
layout (location = 0)in vec3 vVertex;
layout (location = 1)in vec3 vertex_norm;

varying vec3 N;
varying vec3 V;


uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;
uniform vec3 camera;
uniform vec3 vColor;
uniform vec3 lColor;
uniform vec3 lpos_world;

varying mat4 MODEL;
varying mat4 VIEW;
varying mat4 PROJECTION;

varying vec3 lPosition;
varying vec3 vCOLOR;
varying vec3 CAMERA;
varying vec3 lCOLOR;


void main(void) {
    MODEL = vModel;
    VIEW = vView;
    PROJECTION = vProjection;
    
    lPosition = lpos_world;
    vCOLOR = vColor;
    CAMERA = camera;
    lCOLOR = lColor;

    V = vec3(vModel * vec4(vVertex, 1.0));
    N = normalize(vec3(vec4(vertex_norm, 1.0) * inverse(vModel)));
    
    gl_Position = vProjection * vView * vModel * vec4(vVertex, 1.0);
}