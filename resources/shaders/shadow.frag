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


uniform samplerCube mdCube;
uniform float horizontalAOV;
uniform float verticalAOV;

// Input variables coming from vertex shader, interpolated to this fragment
in vec3 interpolatedPosition;
in vec3 interpolatedNormal;

out vec4 fragmentColor;



#define M_PI 3.1415926535897932384626433832795


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

vec3 rotate(vec3 Vx,vec3 axis, float angle) {

  float ca = cos(angle*(M_PI/180));
  float sa = sin(angle*(M_PI/180));

  vec3 cross = cross(Vx,axis);

  float dot= dot(axis,Vx);

  vec3 r=Vx*ca + cross*sa + dot*axis*(1-ca);

  return r;
}

void main()
{

    float depth = gl_FragCoord.z / gl_FragCoord.w;
    vec3 worldPos= WorldPosFromDepth(depth);


    // normal has been interpolated, so we need to normalize it
    vec3 normalVector = normalize(interpolatedNormal);

    // Calculate light source vector
    vec3 lightSourceVector = normalize( interpolatedPosition-lightPos);

    float perpendicular =dot(-lightSourceVector, normalVector );

    vec3 normalColor = vec3(0,0,0);
    if(perpendicular > 0.3)
        normalColor+=vec3(0,perpendicular,0);

    if(perpendicular>=0 && perpendicular <= 0.5)
        normalColor+= vec3(1-perpendicular,0,0);

    if(perpendicular< 0.0)
        normalColor= vec3(1,0,0);

    float stepX = horizontalAOV/20.0;
    float stepY = verticalAOV/20.0;

    for(float xAngle=-horizontalAOV+stepX;xAngle<horizontalAOV;xAngle+=stepX){
        vec3 xRot =rotate(lightSourceVector,vec3(0,1,0),xAngle);
        for(float yAngle=-verticalAOV+stepY;yAngle<verticalAOV;yAngle+=stepY){
            vec3 yRot =rotate(xRot,vec3(1,0,0),yAngle);

            vec4 mdColor= texture(mdCube,yRot);

            if(mdColor.b!=0.0 && mdColor.r==0.0){
                normalColor =vec3(0,0,1);
                break;
            }
       }
    }

    fragmentColor = vec4(normalColor, gl_FragCoord.z);

}
