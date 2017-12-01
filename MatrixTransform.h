#ifndef MATRIXTRANSFORM_H
#define MATRIXTRANSFORM_H

#include "Node.h"

//Goal is to pass down parent matrix to leaf node (aka geometry node) 
class MatrixTransform: public Node {
    
public:
    
    //Specify the position of matrix
    MatrixTransform(glm::mat4); //constructor
    ~MatrixTransform(); //destructor
    
    glm::mat4 MT = glm::mat4(1.0f); //local Matrix coordinates
    
    //vector containing list of Node* to child nodes
    std::vector<Node*> children;
    std::vector<Node*> removedChildren;
    
    void addChild(Node*);
    void removeChild();
    
    bool rotateMode, translateMode, translateBack, scaleMode = false;
    glm::mat4 S, T, R, Proj, View;
    bool isSphere = false;
    glm::vec3 sphereCenter;
    void updateSphereCenter(glm::mat4 M);
    int numAndroid = 0;
    glm::mat4 initSphereMT;
    float radius;
    bool cullIsOn = false;
    bool cullOn = false; 
    
    bool selectionMode = false;
    bool controlOn = false; 
    
    void walk(MatrixTransform* lfMT, MatrixTransform * rfMT);
    void moveArms(MatrixTransform* leftMT, MatrixTransform * rightMT); 
    //multiplies MT (local) with M(matrix passed in from World) aka M*MT
    //passes resulting mat4 down to children
    void update(glm::mat4 M);
    
    
    //traverse list of children and call each child node's draw function
    void draw(GLuint program, glm::mat4 M);
    
    float SIZEARMY; //indicates size of army
    //Build army of Androids
    MatrixTransform * createAndroidArmy(MatrixTransform*, float sizeArmy);
    
    bool frustrumCulling(glm::mat4 P, glm::mat4 V, glm::mat4 sphereMT, float radius, glm::vec3 sphereC);
  
    
    const glm::vec3 planesNormals[6] =
    {
        glm::vec3(0, 0, -1), //Near plane
        glm::vec3(0, 0, 1), //Far plane
        glm::vec3(0, -1, 0), //Top plane
        glm::vec3(0, 1, 0), //Bottom plane
        glm::vec3(-1, 0, 0), //Right plane
        glm::vec3(1, 0, 0) //Left plane
    };
    
    const glm::vec3 planesPoints[6] =
    {
        glm::vec3(0, 0, 1), //Near plane
        glm::vec3(0, 0, -1), //Far plane
        glm::vec3(0, 1, 0), //Top plane
        glm::vec3(0, -1, 0), //Bottom plane
        glm::vec3(1, 0, 0), //Right plane
        glm::vec3(-1, 0, 0) //Left plane
    };
};



#endif
