#include "MatrixTransform.h"



MatrixTransform::MatrixTransform(glm::mat4 M) {
   // MT = MT * M;
    MT = M;

    initSphereMT = M;
    sphereCenter = glm::vec3(0.0, 0.0, 0.0);
}

void MatrixTransform::addChild(Node* nodePtr){
    
    //Add children to list of nodes
    children.push_back(nodePtr);
    
    //update parent
    nodePtr->parent = this;
}

void MatrixTransform::removeChild(){
    removedChildren = children; //store children here
    
    children.clear();
}

//multiplies MT (local) with M(matrix passed in from World) aka M*MT
//passes resulting mat4 down to children
void MatrixTransform::update(glm::mat4 M){
    
    MT = M * MT;
    
    for(unsigned int i = 0; i < children.size(); i++) {
      children[i]->update(MT);
    }
    
}


void MatrixTransform::draw(GLuint program, glm::mat4 M){
    //traverse list of children and call each child node's draw function
    for(unsigned int i = 0; i < children.size(); i++){
        //update the Matrix passed to child
        M = M * MT;
        
        if(selectionMode){
            //send ID for selection buffer to distinguish control points
            GLuint idLocation = glGetUniformLocation(program, "id");
            glUniform1ui(idLocation, i + 1);
        }
        
        children[i]->draw(program, M);
        
    }
}


MatrixTransform* MatrixTransform::createAndroidArmy(MatrixTransform* androidModel, float sizeArmy){
    /** set size of Army**/
    sizeArmy = androidModel->SIZEARMY;
    
    MatrixTransform * ArmyX = new MatrixTransform(glm::mat4(1.0f));
    MatrixTransform * ArmyZ = new MatrixTransform(glm::mat4(1.0f));
    MatrixTransform * androidCopy;
    glm::mat4 translateMatrix;
    
    /** create android copies of sizeArmy in x direction
      * This should create a line of Android
      */
    for (float j = 0; j < sizeArmy; j++) {
        
        translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-300.0f + 60.0*j, -100.0f, -100.0f));
        updateSphereCenter(translateMatrix);//update sphere center

        androidCopy = new MatrixTransform(translateMatrix);
        androidCopy->updateSphereCenter(translateMatrix);
        androidCopy->Proj = Proj;
        androidCopy->View = View;
        //add child androidModel
        androidCopy->addChild(androidModel);
        //add the androidCopy to the Army
        ArmyX->addChild(androidCopy);
        ArmyX->Proj = Proj;
        ArmyX->View = View;
     
    }
    
    /** Using the line of Androids along the X axis. Copies those lines in z direction */
    for (float k = 0; k < sizeArmy; k++) {
        translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f - 60*k));
        //updateSphereCenter(translateMatrix); //update sphere center
        
        androidCopy = new MatrixTransform(translateMatrix);
        androidCopy->updateSphereCenter(translateMatrix);
        androidCopy->Proj = Proj;
        androidCopy->View = View;
        
        //add child androidModel
        androidCopy->addChild(ArmyX);
        //add the androidCopy to the Army
        ArmyZ->addChild(androidCopy);
        ArmyZ->Proj = Proj;
        ArmyZ->View = View;
    }
    
    return ArmyZ;
}


void MatrixTransform::updateSphereCenter(glm::mat4 M){
    
    glm::mat4 sphereMatrix = glm::mat4(1.0);
    sphereMatrix[3][0] = sphereCenter.x;
    sphereMatrix[3][1] = sphereCenter.y;
    sphereMatrix[3][3] = sphereCenter.z;
 
    sphereMatrix = sphereMatrix + M;
    
    sphereCenter.x = sphereMatrix[3][0];
    sphereCenter.y = sphereMatrix[3][1];
    sphereCenter.z = sphereMatrix[3][3];
    
    initSphereMT = M + initSphereMT;
    
    
}


bool MatrixTransform::frustrumCulling(glm::mat4 P ,glm::mat4 V, glm::mat4 sphereMT, float radius, glm::vec3 sphereC){
    

    Proj = P;
    View = V;
    this->radius = radius;
    
    //Get the origin of the sphere (in NDC), using the actual origin.
    glm::vec4 origin = P * V * sphereMT * glm::vec4(sphereC, 1);
    origin = origin /origin.w;
    
    //cout << origin.z << endl;
    //Calculate the axis to offset the radius, which depends on camera rotation and clipping plane direction (in this case (0,0,-1)).
    //Camera rotation is the transpose of the inversed camera rotation. Remember that V=lookAt gives the inversed camera space matrix.
    glm::mat4 inverseCameraRotation = V;
    inverseCameraRotation[3] = glm::vec4(0,0,0,1); //Clear the translation values.
    glm::mat4 cameraRotation = glm::transpose(inverseCameraRotation); //Transpose and inverse gives the same result for orthogonal matrices.
    
    //Axis in camera space.
    for(int i = 0; i < 6 ; i++){
        glm::vec4 axis = cameraRotation * glm::vec4(planesNormals[i], 0); //vec4(n,0) is the unit normal direction of the plane.
        
        //Now, use the axis to find the other side of the radius in NDC space.
        glm::vec4 theOtherSideInWorld = glm::vec4(sphereC, 0) + axis * radius;
        glm::vec4 theOtherSide = P * V * sphereMT * theOtherSideInWorld ;
        theOtherSide = theOtherSide/theOtherSide.w;
        
        //Finally, calculate the radius using the magnitude.
        float radius_in_NDC = glm::length( theOtherSide - origin );
        
    
            //calculate the distance from the origin of the sphere to the plane
            glm::vec3 w = planesPoints[i] - glm::vec3(origin);
            float distance = glm::dot((planesPoints[i]), w) / glm::length(planesPoints[i]);
        
            //check if distance is out of the view frustrum
            //if the distance is less than the radius then it is out of the frustrum
            if (distance < -radius_in_NDC){
                cout << "The android has been culled...." << endl;
                return true;
            }
       
    }

    return false;
}


void MatrixTransform::walk(MatrixTransform* leftMT, MatrixTransform * rightMT){
   // glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 Rforward = glm::rotate(glm::mat4(1.0f), 0.20f, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 Rback = glm::rotate(glm::mat4(1.0f), -0.20f, glm::vec3(1.0f, 0.0f, 0.0f));

   
//    glm::mat4 Rforward = glm::rotate(glm::mat4(1.0f),  cos(1.0f / 180.0f * glm::pi<float>()), glm::vec3(1.0f, 0.0f, 0.0f));
//    glm::mat4 Rback = glm::rotate(glm::mat4(1.0f), cos( 1.0f / 180.0f * glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
   
    glm::mat4 L = leftMT->MT;
    glm::mat4 R = rightMT->MT;
        
        
        auto xL  = L[3][0];
        auto yL  = L[3][1];
        auto zL  = L[3][2];

        auto xR  = R[3][0];
        auto yR  = R[3][1];
        auto zR  = R[3][2];

        L[3][0] = -xL;
        L[3][1] = -yL;
        L[3][2] = -zL;
        R[3][0] = -xR;
        R[3][1] = -yR;
        R[3][2] = -zR;

        L =  L * Rforward;
        R =  R * Rback;

        L[3][0] = xL;
        L[3][1] = yL;
        L[3][2] = zL;
        R[3][0] = xR;
        R[3][1] = yR;
        R[3][2] = zR;


        leftMT->MT =  L;
        rightMT->MT = R;
    
   
}

void MatrixTransform::moveArms(MatrixTransform* leftMT, MatrixTransform * rightMT){
    // glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    //glm::mat4 Rforward = glm::rotate(glm::mat4(1.0f), sin(80.0f / 90.0f ), glm::vec3(0.0f, 10.0f, -10.0f));
    glm::mat4 Rback = glm::rotate(glm::mat4(1.0f), cos(95.0f / 180.0f * glm::pi<float>()), glm::vec3(0.0f, 0.0f, 1.0f));
    
    
    //    glm::mat4 Rforward = glm::rotate(glm::mat4(1.0f),  cos(1.0f / 180.0f * glm::pi<float>()), glm::vec3(1.0f, 0.0f, 0.0f));
    //    glm::mat4 Rback = glm::rotate(glm::mat4(1.0f), cos( 1.0f / 180.0f * glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, -5.0f));
    
    glm::mat4 L = leftMT->MT;
    glm::mat4 R = rightMT->MT;
    
    
    //L =  L * Rforward;
    R =  R * Rback;

    //leftMT->MT =  L;
    rightMT->MT = R;
}

