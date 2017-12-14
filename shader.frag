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
    
     //color = vec4(normalize(Normal), 1.0f);
    // Color everything a hot pink color. An alpha of 1.0f means it is not transparent.
    color = vec4(1.0f, 0.41f, 0.7f, 1.0f);

//    if (material.mode == 3) {
//        color = vec4(normalize(Normal), 1.0f);
//        return;
//    }
//
//    vec3 L;
//    vec3 N = normalize(Normal);
//    float diff;
//    float attenuation;
//    float theta;
//    vec3 camPos = vec3(0.0,0.0, 20.0);
////    float epsilon;
////    float intensity;
//    if(light.mode == 0){
//        L = normalize(-light.direction);
//        diff = max(dot(N, L), 0.0f);
//        attenuation = 1.0;
//        //vec3 = light.color * normalColor;
//       // color = ((light.color * Normal), 1.0f);
//       // color = vec4(light.color, 1.0f);
//    }
//    else if (light.mode == 1 ){
////        distance = length(light.position - fragPos);
////        attenuation = 1.0/(light.cons_att);
//      L = normalize(light.position - fragPos);
//    
//      diff = max(dot(N, L), 0.0f);
//      attenuation = 1.0 / (length(L) * light.linear_att);
//    
//      //diff = max(dot(N, L), 0.0f) * attenuation;
//    }
//    else if (light.mode == 2)
//    {
//        L = normalize(light.position - fragPos);
//        diff = max(dot(N, L), 0.0f);
//    
//        attenuation = 1.0 / (length(L) * length(L) * light.quad_att);
//
//        vec3 lightColor = light.color;
//
//        theta = dot(L, normalize(-light.direction));
//        
//        if(theta < light.cutoff_angle)
//        {
//            lightColor = vec3(0.0f);
//            
//        }
//            
//        float brightness = dot(N, L) / pow((length(L) * length(N)), light.exponent);
//        
//        brightness = clamp(brightness, 0, 1);
//        
//        vec3 ambient = lightColor * material.ambience;
//        //diffuse
//        // calculate the vector from this pixels surface to the light source
//        //L = normalize(light.position - fragPos);
//        //L = normalize(-light.direction);
//        vec3 diffuse = lightColor * (diff * material.diffuse);
//        
//        //specular
//        vec3 viewDir = normalize(camPos - fragPos);
//        vec3 R = 2 * dot(L, N) * N - L;
//        float spec = pow(max(dot(viewDir, R), 0.0), 32.0f);
//        
//        vec3 specular = lightColor * material.specular * spec;
//        
//        vec3 result = (ambient + diffuse + specular) * brightness;
//        color = vec4(result, 1.0f);
//        
//        return;
//    }
//    
//    
//    //ambient
//    vec3 ambient = light.color * material.ambience;
//    //diffuse
//    // calculate the vector from this pixels surface to the light source
//    //L = normalize(light.position - fragPos);
//    //L = normalize(-light.direction);
//    vec3 diffuse = light.color * (diff * material.diffuse);
//    
//    //specular
//    vec3 viewDir = normalize(camPos - fragPos);
//    vec3 R = 2 * dot(L, N) * N - L;
//    float spec = pow(max(dot(viewDir, R), 0.0), 32.0f);
//    vec3 specular = light.color * material.specular * spec;
//    
//    
//    
//    //calculate the cosine of the angle of incidence
//    float brightness = dot(N, L) / (length(L) * length(N));
//    brightness = clamp(brightness, 0, 1);
//    
//    // calculate final color of the pixel, based on:
//    // 1. The angle of incidence: brightness
//    // 2. The color/intensities of the light: light.intensities
//    
//    //    finalColor = vec4(brightness * light.color, 1.0f);
//    
//    vec3 result = (ambient + diffuse + specular) * brightness;
//    color = vec4(result, 1.0f);
//    //color = vec4(0.0f, 0.41f, 0.7f, 1.0f);
//    //color = vec4( 0.75164f, 0.60648f, 0.22648f, 1.0f);
//
//  //  float attenuation = 1.0 / (constantAttenuation + length(L) * linearAttenuation)
    
}
