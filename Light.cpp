
#include "Light.h"
#include "Window.h"
#include "OBJObject.h"

Light::Light(int mode)
{
    toWorld = glm::mat4(1.0f);

    cons_att = 1.0f;
    color = glm::vec3(1.0f);
    //directional lighting
    if (mode == 0){
        color = glm::vec3(1.0f); //white directional light
    }
    //point lighting
    else if (mode == 1) {
        parse("sphere.obj");

    }
    //spot light
    else if (mode == 2) {
        parse("sphere.obj");

    }

    
    // Create array object and buffers. Remember to delete your buffers when the object is destroyed!
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VBO2);
    glGenBuffers(1, &EBO);
    
    // Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
    // Consider the VAO as a container for all your buffers.
    glBindVertexArray(lightVAO);
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
    glVertexAttribPointer(0, 3,GL_FLOAT,GL_FALSE, 3 * sizeof(GLfloat),(GLvoid*)0);
    
    // Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Unbind the VAO now so we don't accidentally tamper with it.
    // NOTE: You must NEVER unbind the element array buffer associated with a VAO!
    glBindVertexArray(0);

}

Light::~Light()
{
    // Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a
    // large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Light::draw(GLuint shaderProgram)
{
    
    //if you are drawing the bunny and then the light, you should be passing the bunny modelview,
    //then drawing the bunny, then passing the light modelview, then drawing the light.
    
    // Calculate the combination of the model and view (camera inverse) matrices
    glm::mat4 modelview = Window::V * toWorld;
    

   // Inverse(LookAt(light_position, light_position + light_direction, vec3(0, 1.0f 0)))
    // We need to calculate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
    // Consequently, we need to forward the projection, view, and model matrices to the shader programs
    // Get the location of the uniform variables "projection" and "modelview"
    uProjection = glGetUniformLocation(shaderProgram, "projection");
    uModelview = glGetUniformLocation(shaderProgram, "modelview");
    // Now send these values to the shader program
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
    // Now draw the object. We simply need to bind the VAO associated with it.
    glBindVertexArray(lightVAO);
    
    //Specify the light mode
    if(lightMode == 0 ){
        renderLight(shaderProgram, 0);
    }
    else if (lightMode == 1)
    {
        renderLight(shaderProgram, 1);
    }
    else if (lightMode == 2)
    {
        renderLight(shaderProgram, 2);
    }
    
    
    // Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    // Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
    glBindVertexArray(0);

    
}
void Light::renderLight(GLuint shaderProgram, int mode){
    
    GLuint lightAttrID = glGetUniformLocation(shaderProgram, "light.mode");
    glUniform1i(lightAttrID, mode);
    
    lightAttrID = glGetUniformLocation(shaderProgram, "light.color");
    //send light color values from CPU to GPU
    glUniform3fv(lightAttrID, 1, &color[0]);
    
    lightAttrID = glGetUniformLocation(shaderProgram, "light.const_att");
    glUniform1i(lightAttrID, cons_att);
    
    //directional lighting
    if (mode == 0){
        //direction
        direction = glm::vec3(-0.2f,-0.0f,-0.3f);
        
        lightAttrID = glGetUniformLocation(shaderProgram, "light.direction");
        glUniform3fv(lightAttrID, 1, &direction[0]);
    }
    //point lighting
    else if (mode == 1) {
        //postition
        position = glm::vec3(-20.0, 10.0, 1.0);
        //Attenuation_linear
        linear_att = 1.0;
        lightAttrID = glGetUniformLocation(shaderProgram, "light.position");
        glUniform3fv(lightAttrID, 1, &direction[0]);
        

        lightAttrID = glGetUniformLocation(shaderProgram, "light.linear_att");
        glUniform1i(lightAttrID, linear_att);
    }
    //spot light
    else if (mode == 2) {
      
        //position
        position = glm::vec3(-20.0, 10.0, 1.0);
        //direction
        direction = glm::normalize(glm::vec3(-0.2f,-1.0f,-0.3f));
        //cutoff
        cutoff_angle = glm::cos(glm::radians(2.5f));
        //exponent
        exponent = 15.0;
        //atten_quad
        quad_att = 9.0;
        
        lightAttrID = glGetUniformLocation(shaderProgram, "light.position");
        glUniform3fv(lightAttrID, 1, &direction[0]);
        
        lightAttrID = glGetUniformLocation(shaderProgram, "light.direction");
        glUniform3fv(lightAttrID, 1, &direction[0]);
        
        lightAttrID = glGetUniformLocation(shaderProgram, "light.cutoff_angle");
        glUniform1i(lightAttrID, cutoff_angle);
        
        lightAttrID = glGetUniformLocation(shaderProgram, "light.exponent");
        glUniform1i(lightAttrID, exponent);
        
        lightAttrID = glGetUniformLocation(shaderProgram, "light.quad_att");
        glUniform1i(lightAttrID, quad_att);
    }
 
}
//parse the OBJ file
void Light::parse(const char *filepath)
{
    //cout << "Begginning to parse.."  << endl;
    
    // Populate the face indices, vertices, and normals vectors with the OBJ Object data
    FILE* fp;  //file pointer
    
    float x,y,z; //vertex coordinates
    unsigned int v1, v2, v3, vn1, vn2, vn3; // index/face
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
        //cout << "checking..." << endl;
        
        //read normal data accordingly
        
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
        else if (c1 == 'v' && c2 == ' ')
        {
            //fscanf() reads data from the stream and stores them according to the parameter format into the locations pointed by the additional arguments
            fscanf(fp, "%f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
            //Store in vec3 vector
            vertex = glm::vec3(x, y, z);
            
            //append vec3 to vertices vector
            vertices.push_back(vertex);
        }
        else if (c1 == 'v' && c2 == 'n')
        {
            fscanf(fp, "%f %f %f", &x, &y, &z);
            //store vec3 vector
            normal = glm::vec3(x,y,z);
            
            //normal = glm::normalize(normal);
            //append normal vector to normals vector
            normals.push_back(normal);
            
        }
        else if (c1 == 'f' && c2 == ' ') {
            
            fscanf(fp, "%u//%u %u//%u %u//%u", &v1, &vn1, &v2, &vn2, &v3, &vn3);
            //make component indices 0-based
            v1 = v1 - 1;
            v2 = v2 - 1;
            v3 = v3 - 1;
            
            indices.push_back(v1);
            indices.push_back(v2);
            indices.push_back(v3);
        }
        
        c1 = fgetc(fp);
        c2 = fgetc(fp);
        
    }
    
//    cout << "Finished parsing..." << endl;
//    cout << "size of vectors.. " << vertices.size() << endl;
//    cout << "size of vnormals.. " << normals.size() << endl;
//    cout << "size of indices.. " << indices.size() << endl;
    
    //close file when done
    fclose(fp);
    glm::mat4 S = scale(glm::mat4(), glm::vec3(0.25,0.25,0.25));
    glm::mat4 T = translate(glm::mat4(), glm::vec3(-20.0, 10.0, 1.0));
    toWorld = S * T * toWorld;
   
}

//void Light::rotate(glm::vec3 rot_axis, float rot_angle){
//    
//    this->toWorld = glm::rotate(glm::mat4(), rot_angle, rot_axis) * this->toWorld;
//    
//    
//}
