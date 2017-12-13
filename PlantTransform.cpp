#include "PlantTransform.h"

PlantTransform::PlantTransform(const char * name,glm::mat4 b) {
	this->name = name;
    this->M = b;
	child = 0;
}

PlantTransform::~PlantTransform() {
	delete(child);
}

void PlantTransform::addChild(Plant * node){
	child = node;
}

void PlantTransform::removeChild(){
	child = 0;
}

void PlantTransform::loadPlant(int n,int plant) {
	switch (n) {
	case 1: child->pos = glm::vec3(0, 50, 0);
		break;
	case 2: child->pos = glm::vec3(0, 50, 0);
		break;
	case 3: child->pos = glm::vec3(0, 48, 0);
		break;
	case 4: child->pos = glm::vec3(0, 46, 0);
		break;
	case 5: child->pos = glm::vec3(0, 38, 0);
		break;
	case 6: 
		child->branch_size = 0.4;
		child->pos = glm::vec3(0, 35, 0);
		break;
	}
	child->loadInstructions(n,plant);
}

void PlantTransform::update(glm::mat4 M) {
	child->pos = child->pos - glm::vec3(0, -5, 0);
}

void PlantTransform::draw(GLuint program, glm::mat4 T){
	child->world = T;
	child->runInstructions(program);
}