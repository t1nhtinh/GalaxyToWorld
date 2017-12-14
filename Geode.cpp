#include "Geode.h"


Geode::Geode(){
   
    
   // if(isSphere){
    //initialize sphere object
//    spherePtr = new OBJObject("/Users/tinhdang/Documents/UCSD/CSE167_IntroToComputerGraphics/Project4_RollerCoaster/Project4_RollerCoaster/sphere.obj");
        
       // MT = objPtr->toWorld;
       // objPtr->matMode = 2;
   // }
    
   
//    
//    //this is the radius of the sphere
//    maxX = objPtr->maxX;
//    maxY = objPtr->minX;
//    maxZ = objPtr->maxZ;
//    radius = objPtr->maxX;
//    //radius = 10.0;
//    cout << "radius of sphere..." << maxX << " " << maxY << endl;
//    sphereCenter = glm::vec3(0.0, 0.0, 0.0);
    MT = glm::mat4(1.0f);
    
}


void Geode::draw(GLuint program, glm::mat4 M){
    
   
    //update matrix to draw object to world and set model matrix
    update(M);
    //objPtr->toWorld = M;
    
    //draw the object file AKA render object
    if(isSphere){
        spherePtr->toWorld = MT;
        spherePtr->drawSphere(program);
        
    }
    
    //draw the curve if it has been initialized
    if(isCurve){
        bezierCurve->drawCurve(program);
    }
    
 
}

//Geode holds the matrix passed down to it AKA MT = M
void Geode::update(glm::mat4 M){
    MT = M;
    //objPtr->toWorld = MT;
    
}
