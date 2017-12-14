#version 330 core

layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aNormal;

out vec3 TexCoords; //position
//out vec3 Normal;

uniform mat4 projection;
uniform mat4 modelview;

void main()
{
    //Normal = mat3(transpose(inverse(modelview))) * aNormal;
    TexCoords = aPos;
    gl_Position = projection * modelview * vec4(aPos, 1.0);

}
