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
out vec4 interpolatedShadow0;
out vec4 interpolatedShadow1;
out vec4 interpolatedShadow2;
out vec4 interpolatedShadow3;
out vec4 interpolatedShadow4;
out vec4 interpolatedShadow5;

void main()
{
    vec3 projCoords;
    mat4 curr_MVP;
    // Transform to eye coordinates
    // The fragment shader values will be the interpolated value
    interpolatedNormal = normalize( N * vertexNormal );
    interpolatedPosition = vec3( MV * vec4( vertexPosition, 1.0 ) );

    if(cameraSim){
        interpolatedShadow0=sim_mvp0 * vec4( vertexPosition, 1.0 );
        interpolatedShadow1=sim_mvp1 * vec4( vertexPosition, 1.0 );
        interpolatedShadow2=sim_mvp2 * vec4( vertexPosition, 1.0 );
        interpolatedShadow3=sim_mvp3 * vec4( vertexPosition, 1.0 );
        interpolatedShadow4=sim_mvp4 * vec4( vertexPosition, 1.0 );
        interpolatedShadow5=sim_mvp5 * vec4( vertexPosition, 1.0 );
    }
    gl_Position = MVP * vec4( vertexPosition, 1.0 );
}
