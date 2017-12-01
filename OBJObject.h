#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB

#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <OpenGL/gl.h> // Remove this line in future projects
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <stdio.h>
#include <iostream>

#include <glm/gtx/transform.hpp> 

using namespace std;

class OBJObject
{
private:

    
    


public:
    std::vector<unsigned int> indices; //faces
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    glm::mat4 toWorld;
    glm::mat4 T; //translation matrix
    glm::mat4 S; //scaling matrix
    
    glm::mat4 R; //rotation matrix


	OBJObject(const char* filepath);

	void parse(const char* filepath);
	void draw(GLuint);
    void drawSpere(GLuint shaderProgram);
    void update(); //update used for spinning
    void spin(float);
    void renderMaterial(GLuint shaderProgram, int); //render material
    int matMode = 0;
    // These variables are needed for the shader program
    GLuint VBO, VBO2, VAO, EBO;
    GLuint uProjection, uModelview, view, model;
    
    
    float angle;
    float pointSize; //size of each point
    float scaleFactor; //scale factor
    bool direction; //direction of transformation, translation
    bool rotateClockwise;
    bool enable; //enable transformation, translation, or rotation
    char flag;
    
    bool rotateMode = false;
    bool translateMode = false;
    bool physiceMode = false; 
    
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
    float avgX, avgY, avgZ;
    
    float center;
    float radius; 
 
    
    //Used to map unit normal [-1,1] to [0,1]
    glm::vec3 mapNormalVectorHelper(glm::vec3 vector);
   
    void increasePointSize(float size);
    void decreasePointSize(float size);
    
    void centerScaleObject(); //recenters and scale object
    void findCenter(); //helper function to get center of model
    void scale(bool);
    void findMinMax(glm::vec3);
    
    void translateX(float);
    void translateY(float);
    void translateZ(bool);

  
    void rotate(glm::vec3, float);
    void reset(bool);
    
    float ID; //unique identifier for each control point
    
};

#endif
