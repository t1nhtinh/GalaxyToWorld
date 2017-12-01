//"Program to an interface"
#ifndef NODE_H
#define NODE_H
#include <vector>
#include "Window.h"

using namespace std;
//common base class for all node types
class Node {
    
    public:
    
        string name;
        Node* parent; //pointer to parent
        
        //TODO need to add bounding box
        
        //pure virtual functions --Abstraction
    
        //update matrix multiplication
        //method to separate bounding sphere updates from rendering
        virtual void update(glm::mat4 M) = 0;
    
        virtual void draw(GLuint program, glm::mat4 M) = 0; //draw objects
    
    
        
    
        //Node constructor
        Node(){}
    
};

#endif
