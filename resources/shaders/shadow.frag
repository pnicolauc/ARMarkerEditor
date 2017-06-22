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

uniform vec3 lightPos;
uniform sampler2D mdTexture0;
uniform sampler2D mdTexture1;
uniform sampler2D mdTexture2;
uniform sampler2D mdTexture3;
uniform sampler2D mdTexture4;
uniform sampler2D mdTexture5;

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
    vec3 lightSourceVector = normalize( lightPos- interpolatedPosition);


    bool biggerX=lightSourceVector.x>0;
    bool biggerY=lightSourceVector.y>0;
    bool biggerZ=lightSourceVector.z>0;

    if(biggerX){
        if(lightSourceVector.y>0){

        }

    }


    vec4 mdColor= texture2D(mdTexture0,gl_FragCoord.xy/500.0);


    float perpendicular =dot( lightSourceVector, normalVector );

    vec3 normalColor = vec3(0,0,0);
    if(perpendicular > 0.3)
        normalColor+=vec3(0,perpendicular,0);

    if(perpendicular>=0 && perpendicular <= 0.5)
        normalColor+= vec3(1-perpendicular,0,0);

    if(perpendicular< 0.0)
        normalColor= vec3(1,0,0);

    //fragmentColor = vec4(mdColor.rg,0, gl_FragCoord.z);
    fragmentColor = vec4(normalColor, gl_FragCoord.z);

}
