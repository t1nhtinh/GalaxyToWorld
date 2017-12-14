#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 projection;
uniform mat4 modelview;

//out vec3 ourColor;
out vec2 TexCoord;
out vec3 Position;

vec3 mapNorm(vec3 vector);

void main()
{
    float dis = length(aPos);
   
    //vec2 pos = normalize(aTexCoord);
   // vec2 pos = vec2(mapNorm(vec3(aTexCoord, 0)));
    
    TexCoord = vec2(mapNorm(normalize(aPos)).x, mapNorm(normalize(aPos)).z);
   // TexCoord = aTexCoord;
    

    //TexCoord = vec2(0.0, 1.0);
    //TexCoord = normalize(vec2(aPos))/dis;
    //TexCoord = normalize(vec2(aPos.x, aPos.z))/dis;
    Position = aPos;
    gl_Position = projection * modelview * vec4(aPos, 1.0);
    
}

vec3 mapNorm(vec3 vector) {
    //extract each vertix
    float x;
    float y;
    float z;
    
    x = (vector.x + 1)/2;
    y = (vector.y + 1)/2;
    z = (vector.z + 1)/2;
    
    vector.x = x;
    vector.y = y;
    vector.z = z;
    
    
    return vector;
}
