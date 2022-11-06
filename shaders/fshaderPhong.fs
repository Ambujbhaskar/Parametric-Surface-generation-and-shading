#version 330 core

varying vec3 N;
varying vec3 V;

varying mat4 MODEL;
varying mat4 VIEW;
varying mat4 PROJECTION;

// vec3 CAMERA = vec3(0.0, 1.5, 3.0);

varying vec3 lPosition;
varying vec3 vCOLOR;
varying vec3 CAMERA;
varying vec3 lCOLOR;

// vec3 COLOR;
float alpha = 32.0;
// vec3 lPosition = vec3(1.0, 1.0, 1.0);

vec3 Ls = vec3(1.0, 1.0, 1.0);
vec3 Ld = vec3(0.7, 0.7, 0.7);
vec3 La = vec3(0.6, 0.2, 0.2);

vec3 ks = vec3(1.0, 1.0, 1.0);
vec3 kd = vec3(0.5, 0.6, 0.4);
vec3 ka = vec3(1.0, 1.0, 1.0);

out vec4 FragColor;

void main(){
    vec3 VERT = V;
    vec3 L = normalize(lPosition - VERT);
    vec3 n = N;
    vec3 v = normalize(vec3(CAMERA));

    vec3 H = normalize(L + v);
    vec3 Ia = ka * La;
    vec3 Id = kd * max(dot(L, n) * Ld, 0);
    vec3 Is = ks * Ls * max(pow(dot(n, H), alpha), 0);

    vec3 nColor = vec3((N.x+1)/2, (N.y+1)/2, (N.z+1)/2);

    FragColor =vec4(vCOLOR * lCOLOR * (Ia + Id + Is), 1.0f);
    // FragColor =vec4(vCOLOR * nColor, 1.0f);
}