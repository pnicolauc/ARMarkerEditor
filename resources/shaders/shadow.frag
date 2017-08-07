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


uniform bool usePreviousCameraSimPos;


uniform sampler2D simTexture0;
uniform sampler2D simTexture1;
uniform sampler2D simTexture2;
uniform sampler2D simTexture3;
uniform sampler2D simTexture4;
uniform sampler2D simTexture5;

in vec4 interpolatedShadow0;
in vec4 interpolatedShadow1;
in vec4 interpolatedShadow2;
in vec4 interpolatedShadow3;
in vec4 interpolatedShadow4;
in vec4 interpolatedShadow5;

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

    if(usePreviousCameraSimPos){
        int count;
        vec4 simVals;
        vec4 currentShadow;
        vec3 projCoords;
        vec2 poissonDisk[4] = vec2[](
          vec2( -0.94201624, -0.39906216 ),
          vec2( 0.94558609, -0.76890725 ),
          vec2( -0.094184101, -0.92938870 ),
          vec2( 0.34495938, 0.29387760 )
        );
        float bias = 0.005;

        for(count=0;count<6;count++){
        simVals = vec4(0,0,0,0);
        vec3 diffuseContribution = vec3(0.0);

        switch(count) {
                   case 0:
                   currentShadow=interpolatedShadow0;
                   break;
                   case 1:
                   currentShadow=interpolatedShadow1;
                   break;
                   case 2:
                   currentShadow=interpolatedShadow2;
                   break;
                   case 3:
                   currentShadow=interpolatedShadow3;
                   break;
                   case 4:
                   currentShadow=interpolatedShadow4;
                   break;
                   case 5:
                   currentShadow=interpolatedShadow5;
                   break;
               }

        projCoords = currentShadow.xyz/ currentShadow.w;
        projCoords = projCoords * 0.5 + 0.5;
        if(projCoords.x<=1.0 && projCoords.x >= 0.0 && projCoords.y<=1.0 && projCoords.y >=0.0 &&  projCoords.z<=1 )
        {

        for (int i=0;i<4;i++){
            switch(count) {
                case 0:
                if ( texture2D( simTexture0, projCoords.xy + poissonDisk[i]/700.0 ).a  >  projCoords.z-bias ){
                  simVals +=vec4(texture2D( simTexture0, projCoords.xy + poissonDisk[i]/700.0 ).rgb,1.0f);
                }
                //simVals = vec4(0,0,0,0);
                break;
                case 1:
                if ( texture2D( simTexture1, projCoords.xy + poissonDisk[i]/700.0 ).a  >  projCoords.z-bias ){
                  simVals +=vec4(texture2D( simTexture1, projCoords.xy + poissonDisk[i]/700.0 ).rgb,1.0f);
                }
                break;
                case 2:
                if ( texture2D( simTexture2, projCoords.xy + poissonDisk[i]/700.0 ).a  >  projCoords.z-bias ){
                  simVals +=vec4(texture2D( simTexture2, projCoords.xy + poissonDisk[i]/700.0 ).rgb,1.0f);
                }
                break;
                case 3:
                if ( texture2D( simTexture3, projCoords.xy + poissonDisk[i]/700.0 ).a  >  projCoords.z-bias ){
                  simVals +=vec4(texture2D( simTexture3, projCoords.xy + poissonDisk[i]/700.0 ).rgb,1.0f);
                }
                break;
                case 4:
                if ( texture2D( simTexture4, projCoords.xy + poissonDisk[i]/700.0 ).a  >  projCoords.z-bias ){
                  simVals +=vec4(texture2D( simTexture4, projCoords.xy + poissonDisk[i]/700.0 ).rgb,1.0f);
                }
                break;
                case 5:
                if ( texture2D( simTexture5, projCoords.xy + poissonDisk[i]/700.0 ).a  >  projCoords.z-bias ){
                  simVals +=vec4(texture2D( simTexture5, projCoords.xy + poissonDisk[i]/700.0 ).rgb,1.0f);
                }
                break;
            }


        }

        simVals/=4;

        if(simVals.r!=0.0 || simVals.g!=0.0 || simVals.b!=0.0 )
            diffuseContribution.rgb = simVals.rgb;


        break;
        }else diffuseContribution = Kd * dot( lightSourceVector, normalVector);

        }
    }

}
