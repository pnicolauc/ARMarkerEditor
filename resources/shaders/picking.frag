#version 330 core

// Light information
uniform vec4 lightPosition;
uniform vec3 lightIntensity;

// Material information
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float shininess;
uniform float id;

// Input variables coming from vertex shader, interpolated to this fragment
in vec3 interpolatedPosition;
in vec3 interpolatedNormal;

out vec4 fragmentColor;

void main()
{
    float marker=1.0;
    float camera=1.0;
    if(id<0.0) camera = -id;
    else marker=id;
    // Calculate final color
    fragmentColor = vec4(marker,camera,gl_FragCoord.w, camera);
}
