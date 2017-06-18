#version 330 core

// Per vertex attributes
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;

// Model View matrix, Normal matrix, and Model View Projection matrix
uniform mat4 MV;
uniform mat3 N;
uniform mat4 MVP;

uniform mat4 sim_mvp0;
uniform mat4 sim_mvp1;
uniform mat4 sim_mvp2;
uniform mat4 sim_mvp3;
uniform mat4 sim_mvp4;
uniform mat4 sim_mvp5;


uniform bool cameraSim;

// Output variables sent to fragment shader
// Interpolated by OpenGL before sent to fragment shader
out vec3 interpolatedNormal;
out vec3 interpolatedPosition;
out vec4 interpolatedShadow;
flat out int simTexture;

void main()
{
    vec3 projCoords;
    mat4 curr_MVP;
    // Transform to eye coordinates
    // The fragment shader values will be the interpolated value
    interpolatedNormal = normalize( N * vertexNormal );
    interpolatedPosition = vec3( MV * vec4( vertexPosition, 1.0 ) );

    if(cameraSim){
        for(int i=0;i<6;i++){
            switch(i) {
                case 0:curr_MVP=sim_mvp0; break;
                case 1:curr_MVP=sim_mvp1; break;
                case 2:curr_MVP=sim_mvp2; break;
                case 3:curr_MVP=sim_mvp3; break;
                case 4:curr_MVP=sim_mvp4; break;
                case 5:curr_MVP=sim_mvp5; break;
            }
            interpolatedShadow=curr_MVP * vec4( vertexPosition, 1.0 );
            projCoords = interpolatedShadow.xyz/ interpolatedShadow.w;
            projCoords = projCoords * 0.5 + 0.5;
            if(projCoords.x<1.0 && projCoords.x > 0.0 &&projCoords.y<1.0 && projCoords.y > 0.0 && projCoords.z<1){
                simTexture=i;
                break;
            }
        }
    }
    gl_Position = MVP * vec4( vertexPosition, 1.0 );
}
