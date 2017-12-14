#ifndef PROCOBJ_H
#define PROCOBJ_H

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

class ProcObj
{
    
public:
    
    ProcObj();
    ~ProcObj();
    
    std::vector<unsigned int> indices; //faces
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> textureCoords;
    std::vector<unsigned int> texIndices;

    std::vector<vector<glm::vec3>> terrain;
    glm::mat4 toWorld;
    GLuint VBO, VAO, EBO, VBO2, VBO3, EBO2;
    GLuint uProjection, uModelview;
  
    glm::vec3 P0, P1, P2, P3;

	int height;
    

    void generateCubeMesh(float w, float h);
    void generateMesh();
    void renderMaterial();
    void activateTexture(GLuint shader, float h);
    unsigned int grassTexture, waterTexture1,  waterTexture2, rockTexture, stoneTexture;
    
    void generateCubeSphere();
    void loadArray();
    void draw(GLuint); 
    int size;
    int numVertices;
    float startPos;
    float randomNum(int min, int max);

    void diamondSquare(int x0, int x1, int y0, int y1, int range);
    void loadVertices(); 
    void squareStep();
    void initCornerPoints();
    void initTerrain();
    
    vector<std::string> images;
    
   
};


const GLfloat texCoords[] = {
    0.0f, 0.0f,  // lower-left corner
    1.0f, 0.0f,  // lower-right corner
    0.0f, 1.0f   // top-center corner
};

//const GLfloat texCoords[3][2] = {
//    {0.0f, 0.0f},  // lower-left corner
//    {1.0f, 1.0f},  // lower-right corner
//    {0.5f, 1.0f}   // top-center corner
//};

#endif
