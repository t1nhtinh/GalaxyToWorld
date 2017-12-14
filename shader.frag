#version 330 core
// This is a fragment shader.


//uniform mat4 model;
// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
//in float sampleExtraOutput;
in vec3 fragPos;
in vec3 Normal;


uniform struct Light   // light
{
    int mode;
    
    vec3 color;
    vec3 position;
    vec3 direction;
    
    float cons_att, linear_att, quad_att;
    float cutoff_angle;
    float exponent;
    
} light;

uniform struct Material {
    int mode;
    vec3 color;
    vec3 ambience; //objects ambient inherent color shines through
    vec3 diffuse; //reflect light arrays in many direction/how much color the light emits
    vec3 specular; //create shiny objects where reflection direction and eye match up
    
} material;


// You can output many things. The first vec4 type output determines the color of the fragment
//out vec4 finalColor
out vec4 color;


void main()
{
color = vec4(1.0f);
}
