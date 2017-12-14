#ifndef PARTICLE_H
#define PARTICLE_H


#include "Window.h"

//Particle properties
class Particle{
   
    public:
    
    glm::mat4 toWorld;
    
    Particle *prev, *next; //link to next and previous particles
    
    glm::vec3 position; //current position
    glm::vec3 velocity; //speed and direction; speed can be combined with direction vector by multiplying
    glm::vec3 color; //vector 4 should be color and opacity
    glm::vec3 prevColor; //last color of particle
    glm::vec3 deltaColor; //change of color
    
    float lifeSpan; //life span of particle

    float size;
    float weight; //if applying gravity
    
    Particle(); //constructor
    ~Particle(); //destructor
    
};


class ParticleSystem{
    
    
    public:
    
    glm::mat4 toWorld;
    GLuint uProjection, uModelview;
    GLuint pathVAO, pathVBO, particleVAO, particleVBO;

    
    ParticleSystem(float r, float num, int mode);
    ~ParticleSystem();
    
    Particle* particlePtr;
    
    std::vector<glm::vec3> particles;
    std::vector<glm::vec3> pathArray; 
    
    void generate(int size, int mode); //initilize/generate the particle system
    void update(float deltaTime); //update particle
    void render(GLuint shaderProgram); //draw the particles
    
    float numParticles;
    int mode;
    
    bool systemOn = false;
    
    float radius; 
};



#endif
