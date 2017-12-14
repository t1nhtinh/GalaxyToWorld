#ifndef PLANT_HPP
#define PLANT_HPP

#include "Node.h"

class RuleSet {
public :
	vector<RuleSet* > rule;
	int value;
};

class Plant {
public:
	glm::vec3 pos;
	float branchingAngle;
	int plantType;
	float branch_size;
	glm::mat4 world;
	bool isGridline;

	Plant( float, float);
	~Plant();

	//L - Systems
	void setupRuleSets();
	void setupFern();
	void setupPlant4();
	void setupBranch();

	vector<RuleSet *> concatenateRules(int n,vector<RuleSet*> *);
	void loadInstructions(int,int);
	void runInstructions(GLuint);

private:
	std::vector<int> instructions;
	vector<GLfloat> vertices;
	vector<GLfloat> normals;
	vector<glm::vec3> positions;

	GLuint VBO, NBO, VAO, EBO;

	void loadBuffers();
	void draw(GLuint,glm::vec3);
	void loadNormalVecs(glm::vec3, glm::vec3, glm::vec3,glm::vec3, glm::vec3, glm::vec3,glm::vec3, glm::vec3);


};
#endif
