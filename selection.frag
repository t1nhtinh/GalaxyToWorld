#version 330 core


uniform uint id;

out vec4 color;

void main()
{
    color = vec4(id/255.0f, 0.0f, 0.0f, 0.0f);
}


