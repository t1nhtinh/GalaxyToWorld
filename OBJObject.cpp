#include "OBJObject.h"
#include "Window.h"
#include "shader.h"

//generic material class
struct Material {
    int mode;
    glm::vec3 color;
    glm::vec3 ambience; //objects ambient inherent color shines through
    glm::vec3 diffuse; //reflect light arrays in many direction/how much color the light emits
    glm::vec3 specular; //create shiny objects where reflection direction and eye match up
    
};

void OBJObject::renderMaterial(GLuint shaderProgram, int mode){
    
    Material mat;
    
    mat.mode = mode;
    //matMode = mode;
    //One of the models should be very shiny, with no diffuse reflection.
//    if (mode == 0) {
//        mat.color = glm::vec3(0.686, 0.933, 0.933);//(0.690, 0.769, 0.871); //light steel blue
//         mat.ambience = glm::vec3(0.25,0.25, 0.25);
//         mat.diffuse = glm::vec3(0.0f);//(0.4f);
//         mat.specular = glm::vec3(.77459, 0.774597, 0.774597);
//    }
//    //Another model should only use diffuse reflection, with zero shininess.
    if (mode == 1)
    {
        mat.color = glm::vec3(0.863, 0.078, 0.235); //crimson red
        mat.ambience = glm::vec3(0.0);//(0.135,0.2225, 0.1574);
        mat.diffuse = glm::vec3(0.54, 0.89, 0.63);
        mat.specular = glm::vec3(0.0);//(0.316228, 0.316228, 0.316228);
    }
    //The third object should have significant diffuse and specular reflection components.
    else if (mode == 2)
    {
        mat.color = glm::vec3(0.133, 0.545, 0.133); //forest green
        //gold
        mat.ambience = glm::vec3(0.24725, 0.19225, 0.19225);
        mat.diffuse = glm::vec3(0.75164, 0.60648, 0.22648);
        mat.specular = glm::vec3(0.628281, 0.555802, 0.366065);
    }
    
    GLuint materialAttr = glGetUniformLocation(shaderProgram, "material.color");
    glUniform3fv(materialAttr, 1, &(mat.color)[0]);
    
    materialAttr = glGetUniformLocation(shaderProgram, "material.ambience");
    //send light color values from CPU to GPU
    glUniform3fv(materialAttr, 1, &(mat.ambience)[0]);
    
    materialAttr = glGetUniformLocation(shaderProgram, "material.diffuse");
    glUniform3fv(materialAttr, 1, &(mat.diffuse)[0]);

    materialAttr = glGetUniformLocation(shaderProgram, "material.specular");
    glUniform3fv(materialAttr, 1, &(mat.specular)[0]);
    
    materialAttr = glGetUniformLocation(shaderProgram, "material.mode");
    glUniform1i(materialAttr, mode);
   
}

//added library for scaling

OBJObject::OBJObject(const char *filepath)
{
	toWorld = glm::mat4(1.0f);
	parse(filepath);
   
    // Create array object and buffers. Remember to delete your buffers when the object is destroyed!
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VBO2);
    glGenBuffers(1, &EBO);
    
    // Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
    // Consider the VAO as a container for all your buffers.
    glBindVertexArray(VAO);
    // bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
    // you want to draw, such as vertices, normals, colors, etc.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
    // the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    
    // We've sent the vertex data over to OpenGL, but there's still something missing.
    // In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* indices.size(), &indices[0], GL_STATIC_DRAW);

    
    // Enable the usage of layout location 0 (check the vertex shader to see what this is)
    glEnableVertexAttribArray(0); //vertex positions
    glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
                          3, // This second line tells us how many components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
                          GL_FLOAT, // What type these components are
                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                          3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          (GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    
    
     //Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
     glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    
    //Bind VBO2 for normal vertices 
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    //Sender buffer data of VBO2
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);

   // glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
    //Enable vertex attribute with position 1
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*) (0));

    
    // Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Unbind the VAO now so we don't accidentally tamper with it.
    // NOTE: You must NEVER unbind the element array buffer associated with a VAO!
    glBindVertexArray(0);
    
    
}

void OBJObject::parse(const char *filepath)
{
	// Populate the face indices, vertices, and normals vectors with the OBJ Object data
    FILE* fp;  //file pointer
   
    float x,y,z; //vertex coordinates
    unsigned int v1, v2, v3, v4, vn1, vn2, vn3, vn4; // index/face
    float r,g,b; //vertex color
    int c1,c2; //characters read from file
    glm::vec3 vertex; //3-compenent vector to store vertex
    glm::vec3 normal; //3-compenent vector to store vertex normals
    glm::vec3 face; //3-compenent vector to store indices AKA faces
    
    fp = fopen(filepath, "rb"); //load file name
    
    if(fp == NULL)
    {
        cerr << "error loading file" << endl;
        exit(-1); //file can not be found or is corrupt
    }
    
    //fgetc() returns the a single character currently pointed to by the internal file position indicator of the specified stream. The internal file position is then advan
    c1 = fgetc(fp);
    c2 = fgetc(fp);
    
    //Read each characer until you reach end of file
    while ( (c1 != EOF) || (c2 != EOF) )
    {
       
        //read normal data accordingly
    //cout <<  (char) c1 << " " << (char) c2 << " " << endl;
        if (c1 == '#' || c2 == '#') {
            //Scan until we reach a new line to update file pointer
            while (c1 != '\n'){
                c1 = fgetc(fp);
                //At the end of the file there isnt no newline so check for EOF
                if (c1 == EOF) {
                    break;
                }
            }
    
        }
        
        if (c1 == 'v' && c2 == ' ')
        {
            //fscanf() reads data from the stream and stores them according to the parameter format into the locations pointed by the additional arguments
            fscanf(fp, "%f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
            //Store in vec3 vector
            vertex = glm::vec3(x, y, z);
            
            //append vec3 to vertices vector
            vertices.push_back(vertex);
            
        
               // c1 = fgetc(fp);
            
        }
        else if (c1 == 'v' && c2 == 'n')
        {
            fscanf(fp, "%f %f %f", &x, &y, &z);
            //store vec3 vector
            normal = glm::vec3(x,y,z);
            
            //normal = glm::normalize(normal);
            //append normal vector to normals vector
            normals.push_back(normal);
            
               // c1 = fgetc(fp);
            
        
        }
        else if (c1 == 'f' && c2 == ' ') {
            
            fscanf(fp, "%u//%u %u//%u %u//%u", &v1, &vn1, &v2, &vn2, &v3, &vn3);
            //make component indices 0-based
            v1 = v1 - 1;
            v2 = v2 - 1;
            v3 = v3 - 1;
            //v4 = v4 - 1;
            
            indices.push_back(v1);
            indices.push_back(v2);
            indices.push_back(v3);
            //indices.push_back(v4);
               // c1 = fgetc(fp);
            
        }
        
//        cout <<  (char) c1 << " " << (char) c2 << " " << endl;
        c1 = fgetc(fp);
        c2 = fgetc(fp);
        
    }
   
    //close file when done
    fclose(fp);
// 
//    cout << "Finished parsing..." << endl;
//        cout << "size of vectors.. " << vertices.size() << endl;
//        cout << "size of vnormals.. " << normals.size() << endl;
//        cout << "size of indices.. " << indices.size() << endl;
//
    
    findCenter(); //center!
    
}

void OBJObject::draw(GLuint shaderProgram)
{

    // Calculate the combination of the model and view (camera inverse) matrices
    glm::mat4 modelview = Window::V * toWorld;
    
    // We need to calculate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
    // Consequently, we need to forward the projection, view, and model matrices to the shader programs
    // Get the location of the uniform variables "projection" and "modelview"
    uProjection = glGetUniformLocation(shaderProgram, "projection");
    uModelview = glGetUniformLocation(shaderProgram, "modelview");
    view = glGetUniformLocation(shaderProgram, "view");
    model = glGetUniformLocation(shaderProgram, "model");
    GLuint camPos = glGetUniformLocation(shaderProgram, "camPos");
    
    // Now send these values to the shader program
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
    glUniformMatrix4fv(view, 1, GL_FALSE, &Window::V[0][0]);
    glUniformMatrix4fv(model, 1, GL_FALSE, &toWorld[0][0]);

    
    // Now draw the object. We simply need to bind the VAO associated with it.
    glBindVertexArray(VAO);
	matMode = 0;
    
    if(matMode == 0){
        renderMaterial(shaderProgram, matMode);
    }
    else if (matMode == 1)
    {
        renderMaterial(shaderProgram, matMode);
    }
    else if (matMode == 2)
    {
        renderMaterial(shaderProgram, matMode);
    }
    else if (matMode == 3) {
        GLuint materialAttr = glGetUniformLocation(shaderProgram, "material.mode");
        glUniform1i(materialAttr, matMode);
    }
    
    // Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    // Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
    glBindVertexArray(0);

}

void OBJObject::drawSphere(GLuint shaderProgram)
{
    // Calculate the combination of the model and view (camera inverse) matrices
    glm::mat4 modelview = Window::V * toWorld;
    
    // We need to calculate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
    // Consequently, we need to forward the projection, view, and model matrices to the shader programs
    // Get the location of the uniform variables "projection" and "modelview"
    uProjection = glGetUniformLocation(shaderProgram, "projection");
    view = glGetUniformLocation(shaderProgram, "view");
    model = glGetUniformLocation(shaderProgram, "model");
    GLuint camPos = glGetUniformLocation(shaderProgram, "camPos");
	GLuint uColor = glGetUniformLocation(shaderProgram, "col");

    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(view, 1, GL_FALSE, &Window::V[0][0]);
    glUniformMatrix4fv(model, 1, GL_FALSE, &toWorld[0][0]);

	glm::vec3 color = glm::vec3(0.125f, 0.698f, 0.667f);
	glUniform3f(uColor, color[0], color[1], color[2]);
    glm::vec3 cam_pos(0.0f, 0.0f, 200.0f);		// e  | Position of camera
    glUniform3f(camPos,cam_pos[0],cam_pos[1],cam_pos[2]);
    
    // Now draw the object. We simply need to bind the VAO associated with it.
    glBindVertexArray(VAO);
    
    // Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
   // glPointSize(1.0f);
  // glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0);
   // glDrawElements(GL_LINE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);
    // Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
    glBindVertexArray(0);
    
}


void OBJObject::spin(float deg)
{

    this->angle += deg;
    if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
    // This creates the matrix to rotate the OBJObject
    this->toWorld = glm::rotate(glm::mat4(1.0f), deg / 180.0f * glm::pi<float>(), glm::vec3(1.0f, -1.0f, 0.0f)) * toWorld;
}

void OBJObject::update()

{
    
    spin(0.5f);
}




void OBJObject::findCenter() {
    //Loop through the vertices to find the minX, minY, minZ and maxX, maxY, maxZ.
    minX = vertices[0].x;
    maxX = vertices[0].x;
    minY = vertices[0].y;
    maxY = vertices[0].y;
    minZ = vertices[0].z;
    maxZ = vertices[0].z;
    
    for(int i = 0; i < vertices.size(); i++) {
       
        if(minX > vertices[i].x){ minX = vertices[i].x; }
        else if(maxX < vertices[i].x){ maxX = vertices[i].x; }
        
        if(minY > vertices[i].y){ minY = vertices[i].y; }
        else if(maxY < vertices[i].y){ maxY = vertices[i].y; }
        
        if(minZ > vertices[i].z){ minZ = vertices[i].z; }
        else if(maxZ < vertices[i].z){ maxZ = vertices[i].z; }
        
    }


    //Find the average: avgX, avgY, avgZ
    avgX = (maxX + minX)/2;
    avgY = (maxY + minY)/2;
    avgZ = (maxZ + minZ)/2;
    
    T = glm::translate(glm::mat4(1.0f), glm::vec3(-avgX, -avgY, -avgZ));
    
    //S will multiply x, y, z with a value: 1/((maximum range of x, y and z)/2).
    auto maxXx = 1/(maxX/2);
    auto maxYy = 1/(maxY/2);
    auto maxZz = 1/(maxZ/2);
    
    S = glm::scale(glm::mat4(1.0f), glm::vec3(maxXx, maxYy, maxZz));
    
    this->toWorld = T * this->toWorld;
   
}


void OBJObject::scale(bool direction)
{
    
    glm::mat4 myScalingMatrix;
    
    if(direction){
        
        myScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    }
    else if (!direction)
    {
        myScalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
    }
    
    this->toWorld = this->toWorld * myScalingMatrix;
}

void OBJObject::translateX(float distance)
{
    glm::mat4 myMatrix;
  
    distance =  10.0 * distance;
   
    myMatrix = glm::translate(glm::mat4(), glm::vec3(distance, 0.0f, 0.0f));

    this->toWorld = myMatrix * this->toWorld ;
    
}

void OBJObject::translateY(float distance)
{
    glm::mat4 myMatrix;
  
    distance = 10.0 * distance;
    
    myMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, distance, 0.0f));

    this->toWorld = myMatrix * this->toWorld ;
}

void OBJObject::translateZ(bool direction)
{
    
    glm::mat4 myMatrix;
    
    
    //move left along x axis by a small smount
    if (direction == 1) {
        myMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -2.0f));
    }
    //move right along x axis by a small smount
    else if (direction == 0)
    {
        myMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 2.0f));
    }
    
    this->toWorld = myMatrix * this->toWorld ;
    
}

void OBJObject::rotate(glm::vec3 rot_axis, float rot_angle){
           
        this->toWorld = glm::rotate(glm::mat4(), rot_angle, rot_axis) * this->toWorld;
    
   
}

void OBJObject::reset(bool resetFlag){
    
    //reset position(move object back to center of screen, without changing orientation or scale factor
    if (resetFlag == 1) {
        //this->toWorld = glm::rotate(glm::mat4(1.0f), this->angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, -1.0f));
        //this->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(-(this->toWorld[3].z), -(this->toWorld[3].z), -(this->toWorld[3].z))) * this->toWorld;
        this->toWorld[3][0] = 0;
        this->toWorld[3][1] = 0;
        this->toWorld[3][2] = 0;
    }
    //reset orientation and scale factor, but leave object where it is
    else if (resetFlag == 0) {
       glm::vec3 position = glm::vec3(this->toWorld[3].z, this->toWorld[3].z, this->toWorld[3].z);
        
        float xComponent = this->toWorld[3][0];
        float yComponent = this->toWorld[3][1];
        float zComponent = this->toWorld[3][2];
        
        this->toWorld = glm::mat4(1.0f);
        this->toWorld[3][0] = xComponent;
        this->toWorld[3][1] = yComponent;
        this->toWorld[3][2] = zComponent;
       
    }
}


glm::vec3 OBJObject::mapNormalVectorHelper(glm::vec3 vector) {
    //extract each vertix
    float x;
    float y;
    float z;
    
    x = (vector.x + 1)/2;
    y = (vector.y + 1)/2;
    z = (vector.z + 1)/2;

    vector.x = x;
    vector.y = y;
    vector.z = z;
    

    return vector;
}
