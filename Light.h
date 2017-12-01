#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "Window.h"
#include "shader.h"

class Light{
    
   
    public:
    
    std::vector<unsigned int> indices; //faces
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    
      //cons_att, linear_att be arbitrary value like 1.0f and 0.09f
        Light(int mode);
        ~Light(); //destructor
   
        glm::mat4 toWorld;
        GLuint uProjection, uModelview;
    
        //int mode;
        void draw(GLuint shaderProgram);
        void parse(const char *filepath); 
    //void rotate(glm::vec3 rot_axis, float rot_angle);
        int lightMode;
        
        glm::vec3 color;
        glm::vec3 position;
        glm::vec3 direction;
        
        float cons_att, linear_att, quad_att;
        float cutoff_angle;
        float exponent;

        GLuint lightVAO, VBO, VBO2, EBO;
    
        void renderLight(GLuint shaderProgram, int mode);
};

#endif
