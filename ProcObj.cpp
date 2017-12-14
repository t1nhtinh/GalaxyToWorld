#include "ProcObj.h"
#include "Window.h"
#include "stb_image.h"
#include <cstdlib>
#include <stdlib.h>
#include <ctime>
#include <cmath>



ProcObj::~ProcObj()
{
 
}



ProcObj::ProcObj(){
    
    toWorld = glm::mat4(1.0f);
       
    //****DiamondSwaure requres size 2^n + 1
    //size = 33; //n = 4
    size = 65; //n = 6
    // size = 513; //n = 9
    
    // numVertices = 289; //17 x 17
    //numVertices = 1089; //33 x 33
    //numVertices = 4225; //65x65
    //numVertices = 263169; //513 x 513
    
    //startPos = 340.0; 17x20
    //startPos = 660.0; 33x20
   // startPos = 20520; 513 x 40
    startPos = 1300; //65 x 20
    
    renderMaterial(); //Load textures for terrain
    
    initTerrain(); //initialize a 2D Terrain Map to represent a height map
    initCornerPoints(); //initialize the 4 corner points
    
    diamondSquare(0, size - 1, 0, size - 1, 120); //generate height map
    generateMesh(); //generate mesh with height map values
    loadArray(); //load vertices, indices, normals from mesh into Vertex array and send to shader
    
    cout << vertices.size() << endl;
    cout << textureCoords.size() << endl;
}

void ProcObj::initTerrain(){
    
    //initialize an array of size 2^n + 1
    glm::vec3 vertex(0.0f);
    vector<glm::vec3> array;
    
    //initialize 2D vector w/ size 2^n + 1 containing zero vectors
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            array.push_back(vertex);
        }
        terrain.push_back(array);
    }
}

void ProcObj::initCornerPoints(){
    //TODO** generate corner points randomly
    
    P0 = glm::vec3(-startPos, 0.0f, -startPos);
    P1 = glm::vec3(startPos, 0.0f, -startPos);
    P2 = glm::vec3(-startPos, 0.0f, startPos);
    P3 = glm::vec3(startPos, 0.0f, startPos);
    
    terrain[0][0] = P0;
    terrain[size - 1][0] = P1;
    terrain[0][size - 1] = P2;
    terrain[size - 1][size - 1] = P3;
}

void ProcObj::diamondSquare(int x0, int x1, int y0, int y1, int range){
    
    int distanceX = x1 - x0;
    int distanceY = y1 - y0;
    int midX = distanceX/2;
    int midY = distanceY/2;
    
    if(distanceX <= 1 || distanceY <= 1) return;

                                    // a     b
                                    //
                                    //    M
                                    //
                                    // c     d

    //The diamond step: For each square in the array, set the midpoint M of that square to be the average of the four corner points plus a random value.
    glm::vec3 a = terrain[x0][y0];
    glm::vec3 b = terrain[x1][y0];
    glm::vec3 c = terrain[x0][y1];
    glm::vec3 d = terrain[x1][y1];
    
    //compute midpoint of square then store in 2D terrain array
    glm::vec3 M = ((a + b + c + d)/ 4.0f) + randomNum(-range, range);
    terrain[x0 + midX][y0 + midY] = M;
    
    
    
                                    //   e
                                    //
                                    //f  M  h
                                    //
                                    //   g
    
    //The square step: For each diamond in the array, set the midpoint of that diamond to be the average of the four corner points plus a random value.
    //Edge Case: During the square steps, points located on the edges of the array will have only three adjacent values set rather than four. There are a number of ways to handle this complication - the simplest being to take the average of just the three adjacent values.
    glm::vec3 e = ((a + b + M + M)/ 4.0f) + randomNum(-range, range);
    glm::vec3 f = ((a + c + M + M)/ 4.0f) + randomNum(-range, range);
    glm::vec3 g = ((b + d + M + M)/ 4.0f) + randomNum(-range, range);
    glm::vec3 h = ((c + c + M + M)/ 4.0f) + randomNum(-range, range);
    terrain[x0 + midX][y0] = e;
    terrain[x0][y0 + midY] = f;
    terrain[x0 + midX][y1] = g;
    terrain[x1][y0 + midY] = h;
    
    
    //At each iteration/recurse call, the magnitude of the random value should be reduced.
    diamondSquare(x0, x0 + midX, y0, y0 + midY, range / 2);   // Upper left
    diamondSquare(x0 + midX, x1, y0, y0 + midY, range / 2);   // Upper right
    diamondSquare(x0, x0 + midX, y0 + midY, y1, range / 2);   // Lower left
    diamondSquare(x0 + midX, x1, y0 + midY, y1, range / 2);   // Lower right
}



void ProcObj::loadVertices(){

    
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            vertices.push_back(terrain[i][j]);
        
        }
    }

    loadArray();
}

void ProcObj::loadArray(){
    
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &VBO2);
    glGenBuffers(1, &VBO3);
    
    // Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
    // Consider the VAO as a container for all your buffers.
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    // Enable the usage of layout location 0 (check the vertex shader to see what this is)
    glEnableVertexAttribArray(0); //change 0 -> 1 for normals
    glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
                          3, // This second line tells us how many components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
                          GL_FLOAT, // What type these components are
                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                          3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          (GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    
    // We've sent the vertex data over to OpenGL, but there's still something missing.
//    // In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* indices.size(), &indices[0], GL_STATIC_DRAW);

    // Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind the VAO now so we don't accidentally tamper with it.
    // NOTE: You must NEVER unbind the element array buffer associated with a VAO!
    glBindVertexArray(0);
    
    //Bind VBO2 for normal vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    //Sender buffer data of VBO2
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
    
    // glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
    //Enable vertex attribute with position 1
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*) (0));
    // Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
    //Load texture coordinates to buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    
    // texture coord attribute
    glBindBuffer(GL_ARRAY_BUFFER, VBO3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * textureCoords.size(), textureCoords.data(), GL_STATIC_DRAW);
    // glBufferData(GL_ARRAY_BUFFER,  sizeof(texCoords), texCoords, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*) (0));
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::vec2) * (textureCoords).size(), textureCoords.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* texIndices.size(), &texIndices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
}


void ProcObj::draw(GLuint shaderProgram){
    glEnable(GL_TEXTURE);
//    diamondSquare(0, size - 1, 0, size - 1, 60); //get height map
//    generateMesh(); //generate mesh with height map
//    loadArray();
    
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

    //activate texture units using glActiveTexture passing in the texture unit we'd like to use
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, waterTexture1);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture0"), 0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, waterTexture2);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 1);
    
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 2);
    
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, rockTexture);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture3"), 3);
    
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, stoneTexture);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture4"), 4);
    
    
    //cout << indices.size() << endl;
     glDrawElements(GL_TRIANGLE_STRIP, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
     //glDrawElements(GL_LINE_STRIP, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
      //glDrawArrays(GL_LINE_STRIP, 0, indices.size());
    // Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
    glBindVertexArray(0);
}


void ProcObj::generateMesh(){
    vertices.clear();
    indices.clear();
    
    float startPt = startPos/2.0; // 340/2
    
    float scale = startPos/size; //  340/17 = 20
    
    int xoffset, yoffset;
    //create nxn grid
    
    yoffset = 0;
    for(float y = -startPt; y < startPt; y = y + scale){
        xoffset = 0;
        for(float x = -startPt; x < startPt; x = x + scale){
            glm::vec3 vertex = glm::vec3(x, terrain[xoffset][yoffset].y, y);
            //cout << terrain[xoffset][yoffset].y << " change in height " << endl;
            vertices.push_back(vertex);
            //xoffset = xoffset + 1;
            
            //update texture coords
            float u = (x / (startPt ));
            float v = (y / (startPt ));
            
            u = (u + 1)/2;
            v = (v + 1)/2;

            glm::vec2 texVertex = glm::vec2(u, v);
            textureCoords.push_back(texVertex);
            
           // cout << u << " " << v << " " << endl;
            xoffset = xoffset + 1;
        }
        yoffset = yoffset + 1;
    }
    
    //draw triangles with indices
    // Build the index data
    int numStrips = size - 1;
    int numDegensRequired = 2 * (numStrips - 1);
    int verticesPerStrip = 2 * size;
    
    int indicesSize = (verticesPerStrip * numStrips) + numDegensRequired;
    
    //initialize indices array
    for(int i = 0; i < indicesSize; i ++){
        indices.push_back(0);
    }
    
    int offset = 0;
    
    for (int y = 0; y < size - 1; y++) {
        if (y > 0) {
            // Degenerate begin: repeat first vertex
            indices[offset++] = (y * size);

        }
        
        for (int x = 0; x < size; x++) {
            // One part of the strip
            indices[offset++] = (y * size) + x;
            indices[offset++] = (((y + 1) * size) + x);
            

        }
        
        if (y < size - 2) {
            // Degenerate end: repeat last vertex
            indices[offset++] = ((y + 1) * size) + (size - 1);

        }
    }
    
    
    int u;
    int v;

    //Generate indices
//    for(int t = 0; t < vertices.size(); t++){
//       
////        u = t % 6;
////        v = (t + 1) % 5;
//        
//        float a = (float)u;
//        float b = (float)v;
//        
//        glm::vec2 texVertex = glm::vec2(u, v);
//        textureCoords.push_back(texVertex);
//
//        texIndices.push_back(u);
//        texIndices.push_back(v);
//        cout << u << " " << v << " " << endl;
//    }

}

void ProcObj::activateTexture(GLuint shaderProgram, float h){
    
}

void ProcObj::renderMaterial(){
    
    // ****step 1 through 3 initialize in setup
    // ****step 4 through 6 should be repeated for each texture in Draw function
    // ****Then make if else statements inside the shader in order to choose the texture you want.

    //        step 1: glTexImage2D should happen first before anything else
    //        step 2: glTexParameteri's
    //        step 3: glGenerateMipmaps
    //        step 4: glActiveTexture
    //        step 5: glBindTexture
    //        step 6: glUniform1i

    glGenTextures(1, &grassTexture); //create reference
    glBindTexture(GL_TEXTURE_2D, grassTexture); //bind reference so any subsequent texture commands will configure to currently bound texture
  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int width, height, nrChannels;
    
    //loops through all 6 texture targets
   // for (unsigned int i = 0; i < faces.size(); i++)
   // {
    unsigned char *data = stbi_load("/Users/tinhdang/Documents/UCSD/CSE167_IntroToComputerGraphics/GalaxyToWorld/GalaxyToWorld/texture/grass.jpg", &width, &height, &nrChannels, 0);
        if (data)
        {
            //Once glTexImage2D is called, the currently bound texture object now has the texture image attached to it.
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            // glGenerateMipmap: automatically generate all the required mipmaps for the currently bound texture.
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "texture failed to load at path: "  << std::endl;
            stbi_image_free(data);
        }
    //}
    
    glGenTextures(1, &waterTexture1);
    glBindTexture(GL_TEXTURE_2D, waterTexture1);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    data = stbi_load("/Users/tinhdang/Documents/UCSD/CSE167_IntroToComputerGraphics/GalaxyToWorld/GalaxyToWorld/texture/water1.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        // Generate mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "texture failed to load at path: "  << std::endl;
        stbi_image_free(data);
    }
    
    glGenTextures(1, &waterTexture2);
    glBindTexture(GL_TEXTURE_2D, waterTexture2);
    
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    data = stbi_load("/Users/tinhdang/Documents/UCSD/CSE167_IntroToComputerGraphics/GalaxyToWorld/GalaxyToWorld/texture/water2.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        // Generate mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "texture failed to load at path: "  << std::endl;
        stbi_image_free(data);
    }
    
    glGenTextures(1, &rockTexture);
    glBindTexture(GL_TEXTURE_2D, rockTexture);
    
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    data = stbi_load("/Users/tinhdang/Documents/UCSD/CSE167_IntroToComputerGraphics/GalaxyToWorld/GalaxyToWorld/texture/dryland.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        // Generate mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "texture failed to load at path: " << std::endl;
        stbi_image_free(data);
    }

    glGenTextures(1, &stoneTexture);
    glBindTexture(GL_TEXTURE_2D, stoneTexture);
    
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    data = stbi_load("/Users/tinhdang/Documents/UCSD/CSE167_IntroToComputerGraphics/GalaxyToWorld/GalaxyToWorld/texture/stone.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        // Generate mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "texture failed to load at path: " << std::endl;
        stbi_image_free(data);
    }


}


void ProcObj::generateCubeMesh(float w, float h){
//    vertices.clear();
//    indices.clear();
    
    float scale = 20.0;
    
    float col = w/ scale;
    
    float startPt = (float)w/10.0;
    
    int index = 0;
    
    int flying = -10;
    
    int yoff = flying;
    for(float y = -startPt; y < startPt; y++){
        
        int xoff = 1;
        
        for(float x = -startPt; x < startPt; x++){
            
           // index++;
            glm::vec3 vertex = glm::vec3(x * scale,  0.0f, y * scale);
            vertices.push_back(vertex);
            
            vertex = glm::vec3(x * scale, 0.0f,  (y + 1.0) * scale);
            vertices.push_back(vertex);
            
            //vertex = glm::vec3((x) * scale, 0.0f,  (y + 1.0) * scale);
           // vertices.push_back(vertex);
            
            //Draw triangle mesh
            indices.push_back(index);
            indices.push_back(index + 1);
           // indices.push_back(index + 2);
            index = index + 1;
           //  cout << " " << vertex.x << " " << vertex.y << " " << vertex.z << endl;
            //randomNum(-10.0, 10.0f)
            
        }
        
        
    }
   
}


                
                

float ProcObj::randomNum(int min, int max){
    int seed;
    srand(seed);
    float randNum;
    
    int dif = max - min;
    //    cout << dif << " The Random" << endl;
    if (dif == 0){
        randNum = 0.0;
        return randNum;
    }
    int num1 = rand() % dif;
    int num2 = num1 + min;
    
    
    
    randNum = (float)num2;
    
    //cout << randNum << " The Random" << endl;
    return randNum;
}


