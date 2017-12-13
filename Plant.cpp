#include "Plant.h"

RuleSet lb;//[
RuleSet rb;//]
RuleSet p;//+
RuleSet m;//-

//Fern Variables and Constants
//Variables
RuleSet x;//X -> F[-&X][X]F[-&X]+^FX
RuleSet f;//F -> FF

//Plant 4 - https://cgjennings.ca/articles/l-systems.html
RuleSet s;
RuleSet z;
RuleSet h;
RuleSet t;
RuleSet l;

//Branch - https://cgjennings.ca/articles/l-systems.html
RuleSet a;
RuleSet b;
RuleSet c;
RuleSet d;
RuleSet e;
RuleSet g;
RuleSet q;
RuleSet i;
RuleSet j;
RuleSet k;

Plant::Plant(float bangle, float bsize) {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &NBO);
	glGenBuffers(1, &EBO);

	const GLuint indices[6][6] = {
		// Front face
		{ 2, 3, 5, 5, 4, 2 },
		// Top face
		{ 4, 5, 7, 7, 6, 4 },
		// Back face
		{ 0, 1, 7, 7, 6, 0 },
		// Bottom face
		{ 2, 3, 1, 1, 0, 2 },
		// Left face
		{ 2, 0, 6, 6, 4, 2 },
		// Right face
		{ 1, 3, 5, 5, 7, 1 }

	};
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	pos = glm::vec3(0);
	branchingAngle = bangle;
	world = glm::mat4(1.0f);
	branch_size = bsize;
	isGridline = false;
}
Plant::~Plant() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &NBO);
	glDeleteBuffers(1, &EBO);
}

 void Plant::setupFern() {
	 //Variables
	x.value = 0;
	x.rule.push_back(&f);
	x.rule.push_back(&lb);
	x.rule.push_back(&m);
	x.rule.push_back(&x);
	x.rule.push_back(&rb);
	x.rule.push_back(&lb);
	x.rule.push_back(&x);
	x.rule.push_back(&rb);
	x.rule.push_back(&f);
	x.rule.push_back(&lb);
	x.rule.push_back(&m);
	x.rule.push_back(&x);
	x.rule.push_back(&rb);
	x.rule.push_back(&p);
	x.rule.push_back(&f);
	x.rule.push_back(&x);

	f.value = 1;
	f.rule.push_back(&f);
	f.rule.push_back(&f);

	//Constants
	p.value = 2;
	p.rule.push_back(&p);

	m.value = 3;
	m.rule.push_back(&m);

	lb.value = 4;
	lb.rule.push_back(&lb);

	rb.value = 5;
	rb.rule.push_back(&rb);
}

 void Plant::setupPlant4() {
	 //Variables
	s.value = 6;
	s.rule.push_back(&lb);
	s.rule.push_back(&p);
	s.rule.push_back(&p);
	s.rule.push_back(&p);
	s.rule.push_back(&z);
	s.rule.push_back(&rb);
	s.rule.push_back(&lb);
	s.rule.push_back(&m);
	s.rule.push_back(&m);
	s.rule.push_back(&m);
	s.rule.push_back(&z);
	s.rule.push_back(&rb);
	s.rule.push_back(&t);
	s.rule.push_back(&s);

	z.value = 6;
	z.rule.push_back(&p);
	z.rule.push_back(&h);
	z.rule.push_back(&lb);
	z.rule.push_back(&m);
	z.rule.push_back(&z);
	z.rule.push_back(&rb);
	z.rule.push_back(&l);

	h.value = 6;
	h.rule.push_back(&m);
	h.rule.push_back(&z);
	h.rule.push_back(&lb);
	h.rule.push_back(&p);
	h.rule.push_back(&h);
	h.rule.push_back(&rb);
	h.rule.push_back(&l);

	t.value = 6;
	t.rule.push_back(&t);
	t.rule.push_back(&l);

	l.value = 6;
	l.rule.push_back(&lb);
	l.rule.push_back(&m);
	l.rule.push_back(&f);
	l.rule.push_back(&f);
	l.rule.push_back(&f);
	l.rule.push_back(&rb);
	l.rule.push_back(&lb);
	l.rule.push_back(&p);
	l.rule.push_back(&f);
	l.rule.push_back(&f);
	l.rule.push_back(&f);
	l.rule.push_back(&rb);
	l.rule.push_back(&f);
}

 void Plant::setupBranch() {
	 //Variables
a.value = 7;
a.rule.push_back(&b);

b.value = 7;
b.rule.push_back(&c);

c.value = 7;
c.rule.push_back(&d);

d.value = 7;
d.rule.push_back(&j);

e.value = 7;
e.rule.push_back(&g);

g.value = 7;
g.rule.push_back(&q);

q.value = 7;
q.rule.push_back(&i);

i.value = 7;
i.rule.push_back(&k);

j.value = 7;
j.rule.push_back(&f);
j.rule.push_back(&lb);
j.rule.push_back(&p);
j.rule.push_back(&a);
j.rule.push_back(&rb);
j.rule.push_back(&f);
j.rule.push_back(&k);

k.value = 7;
k.rule.push_back(&f);
k.rule.push_back(&lb);
k.rule.push_back(&m);
k.rule.push_back(&e);
k.rule.push_back(&rb);
k.rule.push_back(&f);
k.rule.push_back(&j);

}

void Plant::setupRuleSets() {
	setupFern();
	setupPlant4();
	setupBranch();
}
vector<RuleSet *> Plant::concatenateRules(int n,vector<RuleSet *> * buffer) {
	if (n <= 0) {
		return *buffer;
	}
	vector<RuleSet*> output;
	for (int i = 0; i < buffer->size(); i++) {
		output.insert(output.end(),buffer->at(i)->rule.begin(),buffer->at(i)->rule.end());
	}
	n--;
	return Plant::concatenateRules(n,&output);
}
void Plant::loadInstructions(int n,int plnt) {
	instructions.clear();
	vector<RuleSet *> buff;
	plantType = plnt;
	switch (plnt) {
	case 0:
		buff.push_back(&x);
		break;
	case 1:
		buff.push_back(&s);
		buff.push_back(&l);
		buff.push_back(&f);
		buff.push_back(&f);
		buff.push_back(&f);
		break;
	case 2:
		buff.push_back(&p);
		buff.push_back(&p);
		buff.push_back(&j);
	}

	buff = concatenateRules(n,&buff);
	//std::cout << "Size of Buffer: " << buff.size() << std::endl;
	for (int i = 0; i < buff.size(); i++) {
//		std::cout << buff[i]->value;
		instructions.push_back(buff[i]->value);
	}
	buff.clear();
}

void Plant::loadBuffers() {
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0); 

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * normals.size(), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0); 

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Plant::draw(GLuint shader, glm::vec3 color){

	GLuint uProjection = glGetUniformLocation(shader, "projection");
	GLuint view = glGetUniformLocation(shader, "view");
	GLuint model = glGetUniformLocation(shader, "model");
	GLuint uColor = glGetUniformLocation(shader, "col");
	GLuint camPos = glGetUniformLocation(shader, "camPos");
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(view, 1, GL_FALSE, &Window::V[0][0]);
	glUniformMatrix4fv(model, 1, GL_FALSE, &world[0][0]);
	glUniform3f(uColor, color[0], color[1], color[2]);
	glm::vec3 cam_pos(0.0f, 0.0f, 200.0f);		// e  | Position of camera
	glUniform3f(camPos, cam_pos[0], cam_pos[1], cam_pos[2]);

	glBindVertexArray(VAO);

   if (isGridline)
	   glDrawArrays(GL_LINE_STRIP, 0, vertices.size()/3);
   else
	   glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT,0);

   glBindBuffer(GL_ARRAY_BUFFER,0);
   glBindVertexArray(0);
}

void Plant::loadNormalVecs(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, glm::vec3 v5, glm::vec3 v6, glm::vec3 v7, glm::vec3 v8) {
	glm::vec3 output;

	glm::vec3 u = v3 - v1;
	glm::vec3 v = v2 - v1;

	output = glm::cross(u, v);
	normals.push_back(output[0]);
	normals.push_back(output[1]);
	normals.push_back(output[2]);
	//		cout <<"Try Z: "<< glm::to_string(glm::cross(v,u)) << endl;
	//		cout <<"First Normal: "<< glm::to_string(output) << endl;

	u = v1 - v2;
	v = v3 - v2;
	output = glm::cross(u, v);
	normals.push_back(output[0]);
	normals.push_back(output[1]);
	normals.push_back(output[2]);
	//		cout <<"Second: "<< glm::to_string(output) << endl;

	u = v2 - v3;
	v = v1 - v3;
	output = glm::cross(u, v);
	normals.push_back(output[0]);
	normals.push_back(output[1]);
	normals.push_back(output[2]);
	//		cout <<"Third: "<< glm::to_string(output) << endl;

	u = v6 - v4;
	v = v5 - v4;
	output = glm::cross(u, v);
	normals.push_back(output[0]);
	normals.push_back(output[1]);
	normals.push_back(output[2]);
	//		cout <<"Fourth: "<< glm::to_string(output) << endl;

	u = v4 - v5;
	v = v6 - v5;
	output = glm::cross(u, v);
	normals.push_back(output[0]);
	normals.push_back(output[1]);
	normals.push_back(output[2]);
	//		cout <<"Fifth: "<< glm::to_string(output) << endl;

	u = v5 - v6;
	v = v4 - v6;
	output = glm::cross(u, v);
	normals.push_back(output[0]);
	normals.push_back(output[1]);
	normals.push_back(output[2]);
	//	cout <<"Sixth: "<< glm::to_string(output) << endl;

	u = v6 - v7;
	v = v5 - v7;
	output = glm::cross(u, v);
	normals.push_back(output[0]);
	normals.push_back(output[1]);
	normals.push_back(output[2]);
	//	cout <<"Sixth: "<< glm::to_string(output) << endl;

	u = v7 - v8;
	v = v6 - v8;
	output = glm::cross(u, v);
	normals.push_back(output[0]);
	normals.push_back(output[1]);
	normals.push_back(output[2]);
	//	cout <<"Sixth: "<< glm::to_string(output) << endl;

}

void Plant::runInstructions(GLuint shader) {
	glm::vec3 originalPos = pos;
	float originalAngle= branchingAngle;

	bool isLeaf = false;
	stack <glm::vec3> pos_store;
	stack <float> angle_store;

	glm::vec3 p1;
	glm::vec3 p2;
	glm::vec3 p3;
	glm::vec3 p4;
	glm::vec3 p5;
	glm::vec3 p6;
	glm::vec3 p7;
	glm::vec3 p8;
	for (int i = 0; i < instructions.size(); i++) {
		if ((i + 1) < instructions.size()) 
			isLeaf = (instructions[i + 1] == 0) ? true : false;
		switch(instructions[i]) {
		case 1:
			vertices.clear();
			normals.clear();
			vertices.push_back(pos[0]);
			vertices.push_back(pos[1]);
			vertices.push_back(pos[2]);
			p1 = pos;

			//compliment 
			vertices.push_back(pos[0] + 5);
			vertices.push_back(pos[1]);
			vertices.push_back(pos[2]);
			p2 = glm::vec3(pos[0] + 5,pos[1],pos[2]);

			vertices.push_back(pos[0]);
			vertices.push_back(pos[1]);
			pos[2] += branch_size + 5;
			vertices.push_back(pos[2]);
			p3 = pos;

			//compliment 
			vertices.push_back(pos[0] + 5);
			vertices.push_back(pos[1]);
			vertices.push_back(pos[2]);
			p4 = glm::vec3(pos[0] + 5,pos[1],pos[2]);

			vertices.push_back(pos[0]);
			pos[1] += branch_size;
			vertices.push_back(pos[1]);
			vertices.push_back(pos[2]);
			p3 = pos;

			//compliment 
			vertices.push_back(pos[0] + 5);
			vertices.push_back(pos[1]);
			vertices.push_back(pos[2]);
			p6 = glm::vec3(pos[0] + 5,pos[1],pos[2]);


			vertices.push_back(pos[0]);
			vertices.push_back(pos[1]);
			pos[2] -= branch_size + 5;
			vertices.push_back(pos[2]);
			p7 = pos;

			//compliment 
			vertices.push_back(pos[0] + 5);
			vertices.push_back(pos[1]);
			vertices.push_back(pos[2]);
			p8 = glm::vec3(pos[0] + 5,pos[1],pos[2]);

			loadNormalVecs(p1, p2, p3,p4,p5,p6,p7,p8);

			loadBuffers();

			if (isLeaf)
				draw(shader, glm::vec3(0.133, 0.545, 0.133));
			else if (plantType == 1)
				draw(shader, glm::vec3(0.0, 0.392, 0.0));
			else if (plantType == 2)
				draw(shader, glm::vec3(0.878, 1.0, 1.0));
			else
				draw(shader, glm::vec3(0.545, 0.271, 0.075));

			vertices.clear();
			normals.clear();
			break;
		case 2:
			vertices.clear();
			normals.clear();
			vertices.push_back(pos[0]);
			vertices.push_back(pos[1]);
			vertices.push_back(pos[2]);
			p1 = pos;

			//compliment 
			vertices.push_back(pos[0] + 5);
			vertices.push_back(pos[1]);
			vertices.push_back(pos[2]);
			p2 = glm::vec3(pos[0] + 5,pos[1],pos[2]);

			vertices.push_back(pos[0]);
			vertices.push_back(pos[1]);
			pos[2] += branch_size + 5;
			vertices.push_back(pos[2]);
			p3 = pos;
			
			//compliment 
			vertices.push_back(pos[0] + 5);
			vertices.push_back(pos[1]);
			vertices.push_back(pos[2]);
			p4 = glm::vec3(pos[0] + 5,pos[1],pos[2]);

			pos[0] += branch_size*glm::cos(glm::radians(branchingAngle));
			vertices.push_back(pos[0]);
			pos[1] += branch_size*glm::cos(glm::radians(branchingAngle));
			vertices.push_back(pos[1]);
			vertices.push_back(pos[2]);
			p5 = pos;

			//compliment 
			vertices.push_back(pos[0] + 5);
			vertices.push_back(pos[1]);
			vertices.push_back(pos[2]);
			p6 = glm::vec3(pos[0] + 5,pos[1],pos[2]);

			vertices.push_back(pos[0]);
			vertices.push_back(pos[1]);
			pos[2] -= branch_size + 5;
			vertices.push_back(pos[2]);
			p7 = pos;

			//compliment 
			vertices.push_back(pos[0] + 5);
			vertices.push_back(pos[1]);
			vertices.push_back(pos[2]);
			p8 = glm::vec3(pos[0] + 5,pos[1],pos[2]);

			loadNormalVecs(p1, p2, p3,p4,p5,p6,p7,p8);

			loadBuffers();

			if (isLeaf)
				draw(shader, glm::vec3(0.133, 0.545, 0.133));
			else if (plantType == 1)
				draw(shader, glm::vec3(0.0, 0.392, 0.0));
			else if (plantType == 2)
				draw(shader, glm::vec3(0.878, 1.0, 1.0));
			else
				draw(shader, glm::vec3(0.545, 0.271, 0.075));

			vertices.clear();
			normals.clear();
			break;
		case 3:
			vertices.clear();
			normals.clear();
			vertices.push_back(pos[0]);
			vertices.push_back(pos[1]);
			vertices.push_back(pos[2]);
			p1 = pos;

			//compliment 
			vertices.push_back(pos[0] + 5);
			vertices.push_back(pos[1]);
			vertices.push_back(pos[2]);
			p2 = glm::vec3(pos[0] + 5,pos[1],pos[2]);

			vertices.push_back(pos[0]);
			vertices.push_back(pos[1]);
			pos[2] += branch_size + 5;
			vertices.push_back(pos[2]);
			p3 = pos;
			
			//compliment 
			vertices.push_back(pos[0] + 5);
			vertices.push_back(pos[1]);
			vertices.push_back(pos[2]);
			p4 = glm::vec3(pos[0] + 5,pos[1],pos[2]);

			pos[0] -= branch_size*glm::cos(glm::radians(branchingAngle));
			vertices.push_back(pos[0]);
			pos[1] += branch_size*glm::cos(glm::radians(branchingAngle));
			vertices.push_back(pos[1]);
			vertices.push_back(pos[2]);
			p5 = pos;

			//compliment 
			vertices.push_back(pos[0] + 5);
			vertices.push_back(pos[1]);
			vertices.push_back(pos[2]);
			p6 = glm::vec3(pos[0] + 5,pos[1],pos[2]);

			vertices.push_back(pos[0]);
			vertices.push_back(pos[1]);
			pos[2] -= branch_size + 5;
			vertices.push_back(pos[2]);
			p7 = pos;

			//compliment 
			vertices.push_back(pos[0] + 5);
			vertices.push_back(pos[1]);
			vertices.push_back(pos[2]);
			p8 = glm::vec3(pos[0] + 5,pos[1],pos[2]);

			loadNormalVecs(p1, p2, p3,p4,p5,p6,p7,p8);

			loadBuffers();

			if (isLeaf)
				draw(shader, glm::vec3(0.133, 0.545, 0.133));
			else if (plantType == 1)
				draw(shader, glm::vec3(0.0, 0.392, 0.0));
			else if (plantType == 2)
				draw(shader, glm::vec3(0.878, 1.0, 1.0));
			else
				draw(shader, glm::vec3(0.545, 0.271, 0.075));

			vertices.clear();
			normals.clear();
			break;
		case 4:
			pos_store.push(pos);
			angle_store.push(branchingAngle);
			break;
		case 5:
			pos = pos_store.top();
			pos_store.pop();

			branchingAngle = angle_store.top();
			angle_store.pop();

			break;
		}
	}
	pos = originalPos;
	branchingAngle = originalAngle;
	if (!pos_store.empty() || !angle_store.empty() || vertices.size() > 0 || normals.size() > 0) {
		cout << "not empty " << endl;
	}
}
