#ifndef _GEODE_H
#define _GEODE_H
#include "Node.h"
#include "OBJObject.h"
#include "BezierCurve.h"

class Geode: public Node {
 
public:
    
     Geode();
    ~Geode();
    
    BezierCurve * bezierCurve;
    
    glm::mat4 MT; //local matrix coordinate
    
    OBJObject* objPtr; //pointer to OBJObject to be drawn
    OBJObject* spherePtr;
    
    bool isSphere = false;
    bool isCurve = false; 
    bool isOn = false;
    float radius;
    glm::vec3 sphereCenter;
    
    
    float maxX, maxY, maxZ;
    //Geode holds the matrix passed down to it AKA MT = M
    void update(glm::mat4 M);
    
    //call draw on the OBJObject pointer
    void draw(GLuint program, glm::mat4 M);
    
    static int ID;
};

#endif
