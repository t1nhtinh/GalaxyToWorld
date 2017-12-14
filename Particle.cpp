#include <stdlib.h> //rand

#include "Particle.h"

Particle::Particle(){
    
    toWorld = glm::mat4(1.0f);
    color = glm::vec3(0.0f, 0.0f, 0.0f); //start particle to be black then turn to white
    
}

ParticleSystem::ParticleSystem(float r, float num, int m){
    
    particlePtr = new Particle();
    toWorld = glm::mat4(1.0f);
    
    // Create array object and buffers. Remember to delete your buffers when the object is destroyed!
    glGenVertexArrays(1, &pathVAO);
    glGenBuffers(1, &pathVBO);
    
   
    radius = r;
    
    numParticles = num;
    
    mode = m;
   // numParticles = 1000.0f;
    
    generate(numParticles, mode);
    // cout << vertices.size() << endl;
    
    // Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
    // Consider the VAO as a container for all your buffers.
    glBindVertexArray(pathVAO);
    
    // Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
    // you want to draw, such as vertices, normals, colors, etc.
    glBindBuffer(GL_ARRAY_BUFFER, pathVBO);
    // glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
    // the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * pathArray.size(), &pathArray[0], GL_STATIC_DRAW);
    // Enable the usage of layout location 0 (check the vertex shader to see what this is)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

    // Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind the VAO now so we don't accidentally tamper with it.
    // NOTE: You must NEVER unbind the element array buffer associated with a VAO!
    glBindVertexArray(0);
    
    
    //** VAO and VBO for particles
    // Create array object and buffers. Remember to delete your buffers when the object is destroyed!
    glGenVertexArrays(1, &particleVAO);
    glGenBuffers(1, &particleVBO);
    
    
    // Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
    // Consider the VAO as a container for all your buffers.
    glBindVertexArray(particleVAO);
    
    // Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
    // you want to draw, such as vertices, normals, colors, etc.
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    // glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
    // the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * particles.size(), &particles[0], GL_STATIC_DRAW);
    // Enable the usage of layout location 0 (check the vertex shader to see what this is)
    glEnableVertexAttribArray(0); //change 0 -> 1 for normals
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),(GLvoid*)0);
    
    // Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind the VAO now so we don't accidentally tamper with it.
    // NOTE: You must NEVER unbind the element array buffer associated with a VAO!
    glBindVertexArray(0);

}


void ParticleSystem::generate(int size, int mode){
    
    
   // glm::vec3 cen(0.0f,0.0f, 0.0f);
    //vertices.push_back(cen);
    
    
    //Create path for particles
    float rad = radius;
    float angle = 0.0f;
    float circle;
    
    if(mode == 0)
    {
        //** Generate spiral path
        float spiral = radius/ (float)size; //create spiral by decrementing the radius
        circle = (glm::pi<float>() * 2.0)/ (float)size;
        for(int h = 0; h <= size+100; h++){
            glm::vec3 vertex = glm::vec3(radius * cos(angle), radius * sin(angle), -200.0 + (float)(size - 10.0f*h)/2.0f );
            pathArray.push_back(vertex);
            angle = angle + circle;
            radius = radius - spiral;
        }
    }
    else if (mode == 1)
    {
        //**generate belt path xy-plane
        //create two points
        glm::vec3 v1 = glm::vec3(-2000.0f, 2000.0f, -2000.0f);
        glm::vec3 v2 = glm::vec3(2000.0f,  1000.0f,  2000.0f);
        pathArray.push_back(v1);
        pathArray.push_back(v2);
        //get direction between two points
        glm::vec3 direction = v2 - v1;
        direction = direction / (float)(size);
        //create size number of points between the two points
        for(int i = 0; i < size; i++){
            v1 = v1 + direction;
            pathArray.push_back(v1);
        }
    }
    else if (mode == 2)
    {
        rad = 300.0f;
        //**generate ring path in xz-plane
        circle = (glm::pi<float>() * 2.0f)/ ((float)size) ; //evenly space the points to form a circle
        for(int j = 0; j <= size; j++){
            glm::vec3 vertex = glm::vec3( rad * cos(angle), 0.0f, rad * sin(angle) );
            pathArray.push_back(vertex);
            angle = angle + circle;
        }

    }
    
    //** Generate galaxy particles around path
    int size_path = pathArray.size();
//    radius = 800.0;

    //iterate through each point on the path
     for(int k = 0; k < size_path; k++){
         
         glm::vec3 center = pathArray[k];
     
         //generate 100 random points
         for(int l = 0; l < 100; l++){
         
             //get random radius
             int rand_num = rand() % (int)radius + 1; //random number between 1 to radius
             //cout << rand_radius<< " random number" << endl;
             float rand_radius = float(rand_num);
             
             //generate random angle
             int rand_num2 = rand() % 628 + 1; //randum number between 1 and 628 AKA pie
             float rand_angle = (float)rand_num2 / 100.0;
             
             //generate z
             int rand_num3 = rand() % (int)radius + 1; //random number between 1 to radius
             float z = (float)rand_num3;
            
             //generate random sign
             int rand_sign = rand() % 2; //returns either 0 for positive or 1 for negative
             if(rand_sign){
                 z = -z;
             }
        
             //generate x , y, z
             glm::vec3 vertex = center + glm::vec3( rand_radius * cos(rand_angle), rand_radius * sin(rand_angle), z);
             particles.push_back(vertex);
         }
     }
}


void ParticleSystem::render(GLuint shaderProgram){
    
    //draw particle using shader program
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
    glBindVertexArray(pathVAO);
    
    //glPointSize(5.0f);
    
    // Tell OpenGL to draw with points
   // glDrawArrays(GL_POINTS, 0, pathArray.size());

    // Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
    glBindVertexArray(0);
    glBindVertexArray(particleVAO);
    //glPointSize(1.0f);
    // Tell OpenGL to draw with lines
    glDrawArrays(GL_POINTS, 0, particles.size());
   
    // Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
    glBindVertexArray(0);

}


void ParticleSystem::update(float deltaTime){
    //apply physics and particle rotation with respect to time
    
    //for(int i = 0; i < pathArray.size(); i++){
    
    int i = 0;
    glm::vec3 center = pathArray[i];

    for(int j = 0; j < particles.size(); j++){
        
        if(((j + 1) % 100) == 0){
            i++;
            center = pathArray[i];
        }
        
        particles[j] = particles[j] - center;
        
        if(i == (pathArray.size() - 1))
        {
            particles[j] = particles[j] + pathArray[0];
            
        }
        else
        {
           particles[j] = particles[j] + pathArray[i+1];
        }
    }
    
    vector<glm::vec3> tempArray = particles;
    //update particle array
    for(int k = 0; k < particles.size(); k++)
    {
        if((k + 100) == (particles.size())){
            break;
        }
        else
        {
            tempArray[k + 100] = particles[k];
            
            if(k < 100){
                tempArray[k] = particles[(particles.size() - 100) + k];
            }
        }
    }
    
    particles = tempArray;
    
     // Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
    // Consider the VAO as a container for all your buffers.
    glBindVertexArray(particleVAO);
    
    // Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
    // you want to draw, such as vertices, normals, colors, etc.
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    // glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
    // the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * particles.size(), &particles[0], GL_STATIC_DRAW);
    // Enable the usage of layout location 0 (check the vertex shader to see what this is)
    glEnableVertexAttribArray(0); //change 0 -> 1 for normals
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),(GLvoid*)0);
    
    // Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind the VAO now so we don't accidentally tamper with it.
    // NOTE: You must NEVER unbind the element array buffer associated with a VAO!
    glBindVertexArray(0);

}

