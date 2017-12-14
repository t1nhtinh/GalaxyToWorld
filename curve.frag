#version 330 core


uniform struct Material {
    int mode;
    vec3 color;
    vec3 ambience; //objects ambient inherent color shines through
    vec3 diffuse; //reflect light arrays in many direction/how much color the light emits
    vec3 specular; //create shiny objects where reflection direction and eye match up
    
} material;

uniform uint id;

out vec4 color;

void main()
{


    if (material.mode == 0){
        color = vec4(material.color, 1.0f);
        
        return;
    }
    else if (material.mode == 1) {
        color = vec4(material.color, 1.0f);
        
        return;
    }
    else if (material.mode == 2) {
        color = vec4(material.color, 1.0f);
        
        return;
    }
    
    //No material, color pink
     color = vec4(1.0f, 0.41f, 0.7f, 1.0f);

   
}



