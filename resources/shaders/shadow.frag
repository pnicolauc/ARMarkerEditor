#version 330 core

// Light information
uniform vec4 lightPosition;
uniform vec3 lightIntensity;

// Material information
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float shininess;
uniform mat4 P;
uniform mat4 V;

// Input variables coming from vertex shader, interpolated to this fragment
in vec3 interpolatedPosition;
in vec3 interpolatedNormal;

out vec4 fragmentColor;

vec3 WorldPosFromDepth(float depth) {
    float z = gl_FragCoord.z * 2.0 - 1.0;

    float winx = gl_FragCoord.x/500.0f;
    float winy = gl_FragCoord.y/500.0f;
    mat4 invP= inverse(P);

    vec4 clipSpacePosition = vec4(vec2(winx,winy) * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invP * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;
    mat4 invV= inverse(V);

    vec4 worldSpacePosition = invV * viewSpacePosition;

    return worldSpacePosition.xyz;
}


void main()
{
    float depth = gl_FragCoord.z / gl_FragCoord.w;
    vec3 worldPos= WorldPosFromDepth(depth);

    // normal has been interpolated, so we need to normalize it
    vec3 normalVector = normalize(interpolatedNormal);

    // Calculate light source vector
    vec3 lightSourceVector = normalize( lightPosition.xyz - interpolatedPosition);
    float perpendicular =dot( lightSourceVector, normalVector );

    vec3 normalColor = vec3(0,0,0);
    if(perpendicular > 0.3)
        normalColor+=vec3(0,perpendicular,0);

    if(perpendicular>=0 && perpendicular <= 0.5)
        normalColor+= vec3(1-perpendicular,0,0);

    if(perpendicular< 0.0)
        normalColor= vec3(1,0,0);

    fragmentColor = vec4(normalColor, 1.0);
}
