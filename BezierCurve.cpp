#include "BezierCurve.h"

//generic material class
struct Material {
    int mode;
    glm::vec3 color;
};

BezierCurve::BezierCurve(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3){
    
    toWorld = glm::mat4(1.0f);
    
    P0 = p0;
    P1 = p1;
    P2 = p2;
    P3 = p3;

    length = 0;

    // Create array object and buffers. Remember to delete your buffers when the object is destroyed!
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
  
    computeVertices(P0, P1, P2, P3);
   // cout << vertices.size() << endl;
    
    // Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
    // Consider the VAO as a container for all your buffers.
    glBindVertexArray(VAO);
    
    // Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
    // you want to draw, such as vertices, normals, colors, etc.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
    // the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    // Enable the usage of layout location 0 (check the vertex shader to see what this is)
    glEnableVertexAttribArray(0); //change 0 -> 1 for normals
    glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
                          3, // This second line tells us how many components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
                          GL_FLOAT, // What type these components are
                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                          3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          (GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    
//    // We've sent the vertex data over to OpenGL, but there's still something missing.
//    // In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    //cout << "The size of indices..."  << sizeof(indices) << endl;
    // Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind the VAO now so we don't accidentally tamper with it.
    // NOTE: You must NEVER unbind the element array buffer associated with a VAO!
    glBindVertexArray(0);

}

//Convert vector points into matrix form
glm::mat4 BezierCurve::createControlPoint(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3){
    
    glm::mat4 matrix;
    
    //convert row vectors to colums in 4x4 matrix
    matrix[0] = glm::vec4(p0, 1);
    matrix[1] = glm::vec4(p1, 1);
    matrix[2] = glm::vec4(p2, 1);
    matrix[3] = glm::vec4(p3, 1);
    
    //return glm::transpose(matrix);
    return matrix;
}

//Compute Cubic Berstein polynomial: 3Ci * t^i * (1-t)^3-i
//weights should add up to 1 for any value of t
glm::mat4 BezierCurve::computeCubicBerstein(float t){
    
    glm::mat4 leadCoeff;
    
    //cout << "time: " << t << endl;
    //store vector of constants
    vector<float> value;
    
    float v1, v2;
    float threeChooseX = 0;
    
    for(int i = 0; i < 4; i++){
        
        if( i == 0 || i == 3 ){
            threeChooseX = 1;
        }
        else if ( i == 1 || i == 2){
            threeChooseX = 3;
        }
        
        v1 = glm::pow(t, i);
        v2 = glm::pow(1 - t, 3 - i);
        
        //3Ci * t^i * (1-t)^3-i
        float B = threeChooseX * v1 * v2;
        
        //cout << v1 << "" << v2 << endl;
        value.push_back(B);
    }
    
    leadCoeff[0][0] = value[0];
    leadCoeff[0][1] = value[1];
    leadCoeff[0][2] = value[2];
    leadCoeff[0][3] = value[3];
    
    if((value[0] + value[1] + value[2] + value[3]) != 1.0 ){
       // perror("Computing Beinstein did not add up to 1");
        //cout << "Computing Beinstein did not add up to 1" << endl;
    }
    
   // cout << value[0] << " " << value[1] << " " << value[2] <<  " " << value[3] << endl;
   
    return leadCoeff;
    
}

/** Compute the Cubic Bezier Curve
  * output: A row vector3 representing the Bezier Curve
  */
glm::vec3 BezierCurve::createCubicBezierCurve(glm::mat4 points, float t){
    
    glm::mat4 curve;
    glm::vec3 q;
    
    //Compute the leading coefficients
    glm::mat4 coeff = computeCubicBerstein(t);
    
    
    //Compute the the matrix-scaler dot product
    curve = points * coeff;

    q.x = curve[0][0];
    q.y = curve[0][1];
    q.z = curve[0][2];
    
    //cout << q.x << " " << q.y << " " << q.z << endl;
    return q;
}

void BezierCurve::drawCurve(GLuint shaderProgram){
    

    
    // Calculate the combination of the model and view (camera inverse) matrices
    glm::mat4 modelview = Window::V * toWorld;
    // We need to calculate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
    // Consequently, we need to forward the projection, view, and model matrices to the shader programs
    // Get the location of the uniform variables "projection" and "modelview"
    uProjection = glGetUniformLocation(shaderProgram, "projection");
    uModelview = glGetUniformLocation(shaderProgram, "modelview");
    // Now send these values to the shader program
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
    // Now draw the cube. We simply need to bind the VAO associated with it.
    glBindVertexArray(VAO);
    
    
    Material mat;
    
    mat.mode = 0;
    mat.color = glm::vec3(0.628281, 0.555802, 0.366065);
    
    //The third object should have significant diffuse and specular reflection components.
    
    GLuint materialAttr = glGetUniformLocation(shaderProgram, "material.color");
    glUniform3fv(materialAttr, 1, &(mat.color)[0]);
    
    materialAttr = glGetUniformLocation(shaderProgram, "material.mode");
    glUniform1i(materialAttr, mat.mode);
    
    
    // Tell OpenGL to draw with lines
    glDrawArrays(GL_LINE_STRIP, 0, vertices.size());
    
    //glDrawElements(GL_LINES, 36, GL_UNSIGNED_INT, 0);
    // Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
    glBindVertexArray(0);
    
    glBindVertexArray(handlePointVAO);
    
   
    mat.mode = 0;
    mat.color = glm::vec3(0.628281, 0.555802, 0.366065);
//
//    //The third object should have significant diffuse and specular reflection components.
//    
//    GLuint materialAttr = glGetUniformLocation(shaderProgram, "material.color");
    glUniform3fv(materialAttr, 1, &(mat.color)[0]);
    
    materialAttr = glGetUniformLocation(shaderProgram, "material.mode");
    glUniform1i(materialAttr, mat.mode);
    
    
    // Tell OpenGL to draw with lines
    glDrawArrays(GL_LINES, 0, handlePointsArray.size());
    
    //glDrawElements(GL_LINES, 36, GL_UNSIGNED_INT, 0);
    // Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
    glBindVertexArray(0);

}

//draw handlepoints and ensure continuity
//return an array of handle points
vector<glm::vec3> BezierCurve::initHandlePoints(vector<glm::vec3> pointsArray){
   
    //    Simple solution: Let’s make P3/Q0 the midpoint of P2Q1!
    //    P3 (or Q0) = (P2 + Q1) / 2
    //    If P3/Q0 gets moved, move P2 and Q1 the same amount
    //    If P2 gets moved, keep P3/Q0 in place and move Q1 by the negative amount
    //    Do the same thing to P2 if Q1 gets moved
   // handlePointsArray = pointsArray;
    handlePointsArray.clear();
     //store handle points into handle point array
    for (int i = 0; i < 11; i++){
        glm::vec3 midpoint, prev, next;
 
        //Edge case when i = 0
        if(i == 0){
            continue;
        }
        
        else {
          
//            handlePointsArray.push_back(pointsArray[3*i - 1]);
//            handlePointsArray.push_back(pointsArray[3*i + 1]);
            
//            midpoint = (pointsArray[3*i - 1] + pointsArray[3*i + 1])/2.0f;
//            handlePointsArray[3*i] = midpoint;
//            
//            prev = (2.0f*pointsArray[3*i]) - pointsArray[3*i + 1];
//            handlePointsArray[3*i - 1] = prev;
//            
//            next = (2.0f*pointsArray[3*i]) - pointsArray[3*i - 1];
//           // next = (2.0f*midpoint) - pointsArray[3*i - 1];
//            handlePointsArray[3*i + 1] = next;

        }
    
    }
    
//    pointsArray[31] = pointsArray[0];
//    pointsArray[0] =  pointsArray[31];
//    handlePointsArray.push_back(pointsArray[30]);
//    handlePointsArray.push_back(pointsArray[1]);
//    glm::vec3 midpoint, prev, next;
//    
//    //30 31/0 1
//    midpoint = (pointsArray[30] + pointsArray[1])/2.0f;
//    handlePointsArray[0] = midpoint;
//    prev = (2.0f*pointsArray[0]) - pointsArray[1];
//    handlePointsArray[30] = prev;
//    next = (2.0f*pointsArray[0]) - pointsArray[30];
//    handlePointsArray[1] = next;
    

    
    
    //handlePointsArray;
     handlePointsArray = pointsArray;
    
    //cout << pointsArray.size() << " size of points " << endl;
   // cout << handlePointsArray.size() << " size of handle points " << endl;


    
    // Create array object and buffers. Remember to delete your buffers when the object is destroyed!
    glGenVertexArrays(1, &handlePointVAO);
    glGenBuffers(1, &handlePointVBO);
    

    // Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
    // Consider the VAO as a container for all your buffers.
    glBindVertexArray(handlePointVAO);
    
    // Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
    // you want to draw, such as vertices, normals, colors, etc.
    glBindBuffer(GL_ARRAY_BUFFER, handlePointVBO);
    // glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
    // the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * handlePointsArray.size(), &handlePointsArray[0], GL_STATIC_DRAW);
    // Enable the usage of layout location 0 (check the vertex shader to see what this is)
    glEnableVertexAttribArray(0); //change 0 -> 1 for normals
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),(GLvoid*)0);
   
    // Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind the VAO now so we don't accidentally tamper with it.
    // NOTE: You must NEVER unbind the element array buffer associated with a VAO!
    glBindVertexArray(0);
    

    return handlePointsArray;
}

//precondition: handlepoints array is already populated
void BezierCurve::computeVertices(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3){
  
    P0 = p0;
    P1 = p1;
    P2 = p2;
    P3 = p3;
    
    //convert control points into a 4x4 matrix
    pointMatrix = createControlPoint(p0, p1, p2, p3);
    
    vertices.clear();
    
    //store 150 vector points in vector array
    for( float i = 0.0; i < 150.0; i++){
        //time t must be from 0 to 1 and we are drawing 150 lines
        float t = i/ 150.0;
        
        glm::vec3 vertex = createCubicBezierCurve(pointMatrix, t);
        vertices.push_back(vertex);
        
        // cout << vertex.x << " " << vertex.y << " " << vertex.z << endl;
        if( i != 0){
            length = length + glm::length(vertices[i - 1] - vertices[i]);
        }
    }
    

    // Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
    // Consider the VAO as a container for all your buffers.
    glBindVertexArray(VAO);
    
    // Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
    // you want to draw, such as vertices, normals, colors, etc.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
    // the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    // Enable the usage of layout location 0 (check the vertex shader to see what this is)
    glEnableVertexAttribArray(0); //change 0 -> 1 for normals
    glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
                          3, // This second line tells us how many components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
                          GL_FLOAT, // What type these components are
                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                          3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          (GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    
    //    // We've sent the vertex data over to OpenGL, but there's still something missing.
    //    // In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    //cout << "The size of indices..."  << sizeof(indices) << endl;
    // Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind the VAO now so we don't accidentally tamper with it.
    // NOTE: You must NEVER unbind the element array buffer associated with a VAO!
    glBindVertexArray(0);
}


BezierCurve::~BezierCurve() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
