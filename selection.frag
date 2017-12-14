#version 330 core


uniform vec3 col;

in vec3 Normal;
out vec4 color;

void main()
{
     color = vec4(normalize(Normal),1.0f);
//     color = vec4(col, 0.0f);
}


