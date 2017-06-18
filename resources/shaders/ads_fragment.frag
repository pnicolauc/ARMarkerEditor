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


uniform bool cameraSim;


uniform sampler2D simTexture0;
uniform sampler2D simTexture1;
uniform sampler2D simTexture2;
uniform sampler2D simTexture3;
uniform sampler2D simTexture4;
uniform sampler2D simTexture5;




// Input variables coming from vertex shader, interpolated to this fragment
in vec3 interpolatedPosition;
in vec3 interpolatedNormal;
in vec4 interpolatedShadow;
flat in int simTexture;

out vec4 fragmentColor;

void main()
{

    // normal has been interpolated, so we need to normalize it
    vec3 normalVector = normalize(interpolatedNormal);

    // Calculate light source vector
    vec3 lightSourceVector = normalize( lightPosition.xyz - interpolatedPosition);

    // Calculate the view vector
    vec3 viewVector = normalize( -interpolatedPosition.xyz );

    // Ambient contribution
    vec3 ambientContribution = Ka;

    // Default black diffuse and specular contributions
    vec3 diffuseContribution = vec3(0.0);
    vec3 specularContribution = vec3(0.0);

    // Dot product of two normalized vectors gives us a value
    // indicating how close they are to each other,
    // A value of -1 means they are pointing in opposite directions
    // while a value of 1 means they are pointing in the same direction.
    // A value of 0 means they are perpendicular to each other.
    // So if value is negative, there won't be any diffuse or specular contributions
    // since the light source cannot hit those areas
    if( dot( lightSourceVector, normalVector ) > 0.0 ) {

        // Diffuse Contribution
        diffuseContribution = Kd * dot( lightSourceVector, normalVector);

        // halfway vector, reduces the need for calculating a reflected vector,
        // which improvies performance slightly
        vec3 halfwayVector = normalize( viewVector + lightSourceVector);

        // Specular contribution
        specularContribution = Ks * pow( dot(halfwayVector, normalVector), shininess);
    }

    float depth = gl_FragCoord.z / gl_FragCoord.w;
    if(cameraSim){
        vec3 projCoords = interpolatedShadow.xyz/ interpolatedShadow.w;
        projCoords = projCoords * 0.5 + 0.5;

        if(projCoords.x<1.0 || projCoords.x > 0.0 || projCoords.y>1.0 || projCoords.y < 0.0 ||  projCoords.z>1 )
        {
        float bias = 0.005;

        vec2 poissonDisk[4] = vec2[](
          vec2( -0.94201624, -0.39906216 ),
          vec2( 0.94558609, -0.76890725 ),
          vec2( -0.094184101, -0.92938870 ),
          vec2( 0.34495938, 0.29387760 )
        );
        vec4 simVals = vec4(0,0,0,0);
        for (int i=0;i<4;i++){
            switch(simTexture) {
                case 0:
                if ( texture2D( simTexture0, projCoords.xy + poissonDisk[i]/700.0 ).a  >  projCoords.z-bias ){
                  simVals +=vec4(texture2D( simTexture0, projCoords.xy + poissonDisk[i]/700.0 ).rgb,1.0f);
                }
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
                if ( texture2D( simTexture2, projCoords.xy + poissonDisk[i]/700.0 ).a  >  projCoords.z-bias ){
                  simVals +=vec4(texture2D( simTexture2, projCoords.xy + poissonDisk[i]/700.0 ).rgb,1.0f);
                }
                break;
                case 4:
                if ( texture2D( simTexture2, projCoords.xy + poissonDisk[i]/700.0 ).a  >  projCoords.z-bias ){
                  simVals +=vec4(texture2D( simTexture2, projCoords.xy + poissonDisk[i]/700.0 ).rgb,1.0f);
                }
                break;
                case 5:
                if ( texture2D( simTexture2, projCoords.xy + poissonDisk[i]/700.0 ).a  >  projCoords.z-bias ){
                  simVals +=vec4(texture2D( simTexture2, projCoords.xy + poissonDisk[i]/700.0 ).rgb,1.0f);
                }
                break;
            }


        }

        simVals/=5;
        diffuseContribution.rgb = simVals.rgb;

        fragmentColor = vec4(projCoords,1);
        if(projCoords.x>1.0 || projCoords.x < 0.0  )diffuseContribution = Kd * dot( lightSourceVector, normalVector);;
        if(projCoords.y>1.0 || projCoords.y < 0.0  )diffuseContribution = Kd * dot( lightSourceVector, normalVector);;
        if(projCoords.z>1)diffuseContribution = Kd * dot( lightSourceVector, normalVector);
        }
    }
    fragmentColor = vec4(lightIntensity * (ambientContribution + (diffuseContribution)+ specularContribution), 1.0) ;

    //fragmentColor = vec4(lightIntensity * (ambientContribution + (diffuseContribution)+ specularContribution), 1.0) ;

}
