#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
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

#include "Window.h"
#include "OBJObject.h"

class BezierCurve{
    
private:
    
public:
    
    BezierCurve(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
    ~BezierCurve(); //destructor
    
    OBJObject * spherePtr;
    
    glm::mat4 toWorld;
    GLuint uProjection, uModelview;
    GLuint VAO, VBO, handlePointVAO, handlePointVBO;
    
    glm::mat4 pointMatrix;
    
    std::vector<glm::vec3> vertices; //store vertices
    std::vector<glm::vec3> handlePointsArray; //store handle Points
    
    glm::mat4 createControlPoint(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p4);
    
    glm::mat4 computeCubicBerstein(float t);
    
    glm::vec3 createCubicBezierCurve(glm::mat4 points, float t);
    
    void drawCurve(GLuint shaderprogram);
    vector<glm::vec3> initHandlePoints(vector<glm::vec3> pointsArray);
    void selectionDraw(GLuint shaderProgram);
    void computeVertices(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
    glm::vec3 P0, P1, P2, P3;
    
    float velocity; 
    
    float length; 
};


#endif
