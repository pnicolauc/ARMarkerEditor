#version 330 core

// Light information
uniform vec4 lightPosition;
uniform vec3 lightIntensity;
uniform sampler2D texture;
// Material information
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float shininess;

uniform bool selected;

#define TOP 0.95
#define BOTTOM 0.05


// Input variables coming from vertex shader, interpolated to this fragment
in vec3 interpolatedPosition;
in vec3 interpolatedNormal;
in vec2 texC;
out vec4 fragmentColor;

void main()
{
    // Calculate final color
    if(selected){
        vec4 color=vec4(0,0,0,1);

        if(texC.x>TOP) color.b=texC.x*10;
        else if(texC.x<BOTTOM)  color.b=(1-texC.x)*10;
        else if(texC.y>TOP) color.b=texC.y*10;
        else if(texC.y<BOTTOM)  color.b=(1-texC.y)*10;
        else color= texture2D(texture, texC);

        fragmentColor = vec4(color);
    }
    else fragmentColor = texture2D(texture, texC);
}
