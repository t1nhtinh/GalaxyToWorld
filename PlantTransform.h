#ifndef PLANTTRANSFORM_H
#define PLANTTRANSFORM_H

#include "Node.h"
#include "Plant.h"

class PlantTransform :public Node
{
public:
	const char* name;
	Plant * child;
	glm::mat4 M;
	PlantTransform(const char*,glm::mat4 b);

	~PlantTransform();

	void addChild(Plant * node);

	void removeChild();

	void loadPlant(int,int);

	void update(glm::mat4 M);

	void draw(GLuint program, glm::mat4 M);
};
#endif