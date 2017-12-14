#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position; //vertex
layout (location = 1) in vec3 aNormal;//1 for normals
//layout (location = 2) in vec3 lightPosition;
//layout (location = 3) in vec3 lightNormal;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 model;
uniform mat4 view;


// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
// The default output, gl_Position, should be assigned something. You can define as many
// extra outputs as you need.
//out float sampleExtraOutput;
out vec3 fragPos;
out vec3 Normal;
//out vec3 eye;
//out Light light;
//out Material material;

void main()
{
    //pass variables to fragment shader
   // Normal = aNormal;
    //fragPos = vec3(model * vec4(position, 1.0);
    //fragPos = position;
    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0);
   // gl_Position = projection * modelview * vec4(lightPosition.x, lightPosition.y, lightPosition.z, 1.0);
   
    // calculate the location of this fragment (pixel) in world coordinates
    //    vec3 fragPosition = vec3(model * vec4(fragPos, 1));
    fragPos = vec3(model * vec4(position,1.0));
    
    // calculate normal in world coordinates
    //TODO****Recommend to do calculation in CPU then send to GPU
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalize(normalMatrix * aNormal);
    
  
    //Calculate point with respect to camera space
    //eye = -(modelview * position);

}
