#version 330 core

out vec4 FragColor;

//in vec3 ourColor;
in vec2 TexCoord;
in vec3 Position;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;

void main()
{
    float h = Position.y;
    
    //render water
    if (h < 0.0) {
        FragColor =  mix(texture(texture0, TexCoord), texture(texture1, TexCoord), 0.5);
        //FragColor = texture(texture0, TexCoord);
    }
    //render land maters
    else if (h < 5.0) {
        FragColor = texture(texture2, TexCoord);
    }
    else if (h < 30){
        //FragColor = texture(texture2, TexCoord);
        FragColor =  mix(texture(texture2, TexCoord), texture(texture3, TexCoord), 0.4);
    }
    //render land
    else if (h < 70.0) {
        FragColor = texture(texture3, TexCoord);
        //FragColor =  mix(texture(texture2, TexCoord), texture(texture3, TexCoord), 0.2);
    }
    else if (h < 90.0) {
               // FragColor = texture(texture4, TexCoord);
        FragColor =  mix(texture(texture3, TexCoord), texture(texture4, TexCoord), 0.6);
    }
    //render stone caps
    else{
        FragColor = texture(texture4, TexCoord);
    }
    
}
