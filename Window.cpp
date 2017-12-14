#include "Window.h"
#include "Node.h"
#include "MatrixTransform.h"
#include "Plant.h"
#include "PlantTransform.h"
#include "Geode.h"
#include "ProcObj.h"
#include <time.h>

const char* window_title = "Galaxy";
Cube * cube;
Cube * cubeSphere;
GLint shaderProgram, skyboxShaderProgram, toonShaderProgram, selectionShaderProgram;
GLint particleShaderProgram, terrainShader;

OBJObject * objectPtr;
OBJObject * bunny2;
OBJObject * dragon;
OBJObject * bear;
OBJObject * coaster;
OBJObject * sunSphere;
OBJObject * planet1;
OBJObject * planet2;
OBJObject * bird;
OBJObject * bird2;
OBJObject * bird3;
OBJObject * bird4;

#define VERTEX_SHADER_PATH "../shader.vert"
#define FRAGMENT_SHADER_PATH "../shader.frag"

#define VERTEX_SKYBOXSHADER "../skyboxShader.vert"
#define FRAGMENT_SKYBOXSHADER "../skyboxShader.frag"

#define VERTEX_TOONSHADER "../environmentshader.vert"
#define FRAGMENT_TOONSHADER "../environmentshader.frag"

#define VERTEX_SELECTIONSHADER "../selection.vert"
#define FRAGMENT_SELECTIONSHADER "../selection.frag"

#define VERTEX_PARTICLESHADER "../particleShader.vert"
#define FRAGMENT_PARTICLESHADER "../particleShader.frag"

#define VERTEX_TERRAINSHADER "../terrainShader.vert"
#define FRAGMENT_TERRAINSHADER "../terrainShader.frag"

// Default camera parameters
glm::vec3 cam_pos(0.0f, 100.0f, 600.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is
glm::vec3 prev_position;
glm::vec3 curr_position;
double curr_x, curr_y;

int Window::width;
int Window::height;

glm::mat4 Window::P;
glm::mat4 Window::V;


ParticleSystem* beltPtr;
ParticleSystem* ringPtr;
ParticleSystem * birdPathPtr;
//ParticleSystem* spiralPtr;

ProcObj* procObj;


int curveCount = 0; //8 curves
int lineCount = 0; //150 line segments per curve

int startCurve, startLine;

static float a = 0.00005f; //acceleration
static float c = 0.005f; //some constant to push the sphere
float velocity;
float maxH = 0.0; //max height
glm::vec3 start; //position coaster at the highest point
float deltaH = 0.0; //max height of track - current height of sphere
float currH = 0.0;
int id;

int particleTimer = 0;

glm::vec3 sun;
bool lButtonDown;
glm::vec3 prevPos;
bool tZ;

//L-System Plants
vector<PlantTransform*> plants;
int drawCounter = 0;

Plant * fern;
vector<PlantTransform*> ferns;
int fernGrowth;

Plant * plant4;
vector<PlantTransform*> plant4s;
int plant4Growth;

Plant * branch;
vector<PlantTransform*> branches;
int branchGrowth;

//Belt Rider
int beltIndex;
bool travelEnd;
bool travel;
bool ringTravel;
bool seeBunny;

vector<glm::vec3> birdCoaster;
int birdIndex;

void Window::initialize_objects()
{
	birdIndex = 0;
	beltIndex = 40;
	travel = false;
	travelEnd = false;
	lButtonDown = false;

	cube = new Cube();
    
	// Load the shader program. Make sure you have the correct filepath up top
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
    skyboxShaderProgram = LoadShaders(VERTEX_SKYBOXSHADER, FRAGMENT_SKYBOXSHADER);
	toonShaderProgram = LoadShaders(VERTEX_TOONSHADER, FRAGMENT_TOONSHADER);
    selectionShaderProgram = LoadShaders(VERTEX_SELECTIONSHADER, FRAGMENT_SELECTIONSHADER);
    terrainShader = LoadShaders(VERTEX_TERRAINSHADER, FRAGMENT_TERRAINSHADER);
    particleShaderProgram = LoadShaders(VERTEX_PARTICLESHADER, FRAGMENT_PARTICLESHADER);
    
    //Create the roller coaster using a sphere
    coaster = new OBJObject ("sphere.obj");
    
    glm::mat4 scaleCoaster = glm::scale((glm::mat4(1.0f), glm::vec3(100.0f)));
    coaster->toWorld = scaleCoaster * coaster->toWorld;
    coaster->toWorld[3] = glm::vec4(cam_look_at, 1.0f);

    objectPtr = coaster;
    objectPtr->matMode = 0;

	initializeFerns(6);
	fernGrowth = 1;
	initializePlant4s(6);
	plant4Growth = 1;
	initializeBranch(6);
	branchGrowth = 1;

	//Initialize the sun
	sunSphere = new OBJObject("sphere.obj");
	sun = glm::vec3(glm::vec3(415, 317, 438));
	sunSphere->toWorld[3] = glm::vec4(sun,1.0f);
	tZ = false;// switch for control over camera or sun

	float rad = coaster->maxX;
	rad = rad * 50.0f;
	cubeSphere = new Cube();
	cubeSphere->toWorld = glm::scale((glm::mat4(1.0f), glm::vec3(0.5f))) * cubeSphere->toWorld;
	cubeSphere->toWorld = glm::scale((glm::mat4(1.0f), glm::vec3(rad))) * cubeSphere->toWorld;

	//spiralPtr = new ParticleSystem(100.0f, 1000.0f, 0);
	
//	spiralPtr->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -100.0f, 0.0f)) * spiralPtr->toWorld;
	beltPtr = new ParticleSystem(800.0f, 1000.0f, 1);
	ringPtr = new ParticleSystem(25.0f, 800.0f, 2);
	birdPathPtr = new ParticleSystem(10.0f, 600.0f, 2);


	bird = new OBJObject("dove.obj");
    bird->toWorld[3] = glm::vec4(cam_look_at, 1.0f);
	bird->toWorld = glm::translate(glm::mat4(1.0f),glm::vec3(-50,0,0)) * glm::scale(glm::mat4(1.0f),glm::vec3(300.0f)) *bird->toWorld;

	bird2 = new OBJObject("dove.obj");
    bird2->toWorld[3] = glm::vec4(cam_look_at, 1.0f);
	bird2->toWorld = glm::translate(glm::mat4(1.0f),glm::vec3(50,0,0)) * glm::scale(glm::mat4(1.0f),glm::vec3(300.0f)) *bird2->toWorld;

	bird3 = new OBJObject("dove.obj");
    bird3->toWorld[3] = glm::vec4(cam_look_at, 1.0f);
	bird3->toWorld = glm::translate(glm::mat4(1.0f),glm::vec3(0,30,0)) * glm::scale(glm::mat4(1.0f),glm::vec3(300.0f)) *bird3->toWorld;

	bird4 = new OBJObject("dove.obj");
    bird4->toWorld[3] = glm::vec4(cam_look_at, 1.0f);
	bird4->toWorld = glm::translate(glm::mat4(1.0f),glm::vec3(0,-30,0)) * glm::scale(glm::mat4(1.0f),glm::vec3(300.0f)) *bird4->toWorld;
	//particlePtr->toWorld[3] = glm::vec4(100.0f);

	procObj = new ProcObj();
	procObj->toWorld = glm::translate(glm::mat4(), glm::vec3(0.0f, -800.0f, -1000.0f)) *  procObj->toWorld;

	glm::mat4 transPlanet = glm::translate(glm::mat4(1.0f), glm::vec3(-300.0f, 200.0f, -600.0f));
	glm::mat4 transPlanet2 = glm::translate(glm::mat4(1.0f), glm::vec3(400.0f, -100.0f, 600.0f));

	glm::mat4 planet1Scale = glm::scale(glm::mat4(1.0f), glm::vec3(150.0f));
	planet1 = new OBJObject("sphere.obj");
    planet1->toWorld[3] = glm::vec4(cam_look_at, 1.0f);
	planet1->toWorld = transPlanet * planet1Scale * planet1->toWorld;

	birdPathPtr->toWorld = transPlanet * birdPathPtr->toWorld;
	birdCoaster = birdPathPtr->pathArray;

	planet2 = new OBJObject("sphere.obj");
    planet2->toWorld[3] = glm::vec4(cam_look_at, 1.0f);
	planet2->toWorld = transPlanet2 * scaleCoaster * planet2->toWorld;

	glm::mat4 transBunny2 = glm::translate(glm::mat4(1.0f), glm::vec3(150.0f, 08.0f, 380.0f));

	bunny2 = new OBJObject("bunny.obj");
	bunny2->toWorld[3] = glm::vec4(cam_look_at, 1.0f);
	bunny2->toWorld = transBunny2* scaleCoaster * bunny2->toWorld;

	seeBunny = true;


}

void Window::initializeBranch(int n) {
	//Branch style plants setup 
	branch = new Plant(5,10.0f);

	for (int i = 0; i < n; i++) {
		PlantTransform * p = new PlantTransform("branch", objectPtr->toWorld);
		p->addChild(branch);
		p->loadPlant(1,2);//initialize with height of 1
		plants.push_back(p);
		branches.push_back(p);
	}
}
void Window::drawBranches(GLint shader) {
	glUseProgram(shader);
	GLuint uLightPos = glGetUniformLocation(shader, "lightPosInput");
	glUniform3f(uLightPos, sun[0], sun[1], sun[2]);

	glm::mat4 tempWorld = glm::scale(glm::mat4(1), glm::vec3(1.0f / 50.0f)) * objectPtr->toWorld;
	glm::mat4 inverseRotateWorld = glm::transpose(tempWorld);
	float yOffset;
	glm::mat4 scaleMat;

	if (branchGrowth - 1 < 2) {
	//	cout << "Inside: " << plant4Growth << endl;
		scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0/5.0f));
		yOffset = 200.0f;
	}
		else if (branchGrowth - 1 == 2) {
			scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0 / 5.0f));
			yOffset = 200.0f;
		}
		else if (branchGrowth - 1 == 3) {
			scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0 / 7.0f));
			yOffset = 305.0f;
		}
		else if (branchGrowth - 1 == 4) {
			scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0 / 9.0f));
			yOffset = 407.0f;
		}
		else if (branchGrowth - 1 == 5) {
			scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0 / 11.0f));
			yOffset = 500.0f;
		}

	float rot_on_sphere = -60;
	float tran_on_sphere = 0;

	tempWorld = glm::rotate(glm::mat4(1.0f),
		glm::radians(rot_on_sphere), glm::vec3(0, 0, 1))
		*  inverseRotateWorld;
	tempWorld = glm::transpose(tempWorld) * glm::translate(glm::mat4(1.0f),
		glm::vec3(0, yOffset, 0));
	branches[0]->draw(shader,scaleMat * tempWorld);

	rot_on_sphere = -30;
	tran_on_sphere = -10;

	tempWorld = glm::rotate(glm::mat4(1.0f),
		glm::radians(rot_on_sphere), glm::vec3(0, 0, 1))
		*  inverseRotateWorld;
	tempWorld = glm::transpose(tempWorld) * glm::translate(glm::mat4(1.0f),
		glm::vec3(0,yOffset,tran_on_sphere));
	branches[1]->draw(shader,scaleMat * tempWorld);

	rot_on_sphere = 10;
	tran_on_sphere = -30;

	tempWorld = glm::rotate(glm::mat4(1.0f),
		glm::radians(rot_on_sphere), glm::vec3(0, 0, 1))
		*  inverseRotateWorld;
	tempWorld = glm::transpose(tempWorld) * glm::translate(glm::mat4(1.0f),
		glm::vec3(0,yOffset,tran_on_sphere));
	branches[2]->draw(shader,scaleMat * tempWorld);

	rot_on_sphere = 50;
	tran_on_sphere = -20;

	tempWorld = glm::rotate(glm::mat4(1.0f),
		glm::radians(rot_on_sphere), glm::vec3(0, 0, 1))
		*  inverseRotateWorld;
	tempWorld = glm::transpose(tempWorld) * glm::translate(glm::mat4(1.0f),
		glm::vec3(0,yOffset,tran_on_sphere));
	branches[3]->draw(shader,scaleMat * tempWorld);

	rot_on_sphere = -100;
	tran_on_sphere = 0;

	tempWorld = glm::rotate(glm::mat4(1.0f),
		glm::radians(rot_on_sphere), glm::vec3(0, 0, 1))
		*  inverseRotateWorld;
	tempWorld = glm::transpose(tempWorld) * glm::translate(glm::mat4(1.0f),
		glm::vec3(0, yOffset, 0));
	branches[4]->draw(shader,scaleMat * tempWorld);

	rot_on_sphere = 80;
	tran_on_sphere = -10;

	tempWorld = glm::rotate(glm::mat4(1.0f),
		glm::radians(rot_on_sphere), glm::vec3(0, 0, 1))
		*  inverseRotateWorld;
	tempWorld = glm::transpose(tempWorld) * glm::translate(glm::mat4(1.0f),
		glm::vec3(0,yOffset,tran_on_sphere));
	branches[5]->draw(shader,scaleMat * tempWorld);
}

void Window::initializePlant4s(int n) {
	//Plant 4 style plants setup 
	plant4 = new Plant(5,10.0f);

	for (int i = 0; i < n; i++) {
		PlantTransform * p = new PlantTransform("plant4 ", objectPtr->toWorld);
		p->addChild(plant4);
		p->loadPlant(1,1);
		plants.push_back(p);
		plant4s.push_back(p);
	}
}
void Window::drawPlant4s(GLint shader) {
	glUseProgram(shader);
	GLuint uLightPos = glGetUniformLocation(shader, "lightPosInput");
	glUniform3f(uLightPos, sun[0], sun[1], sun[2]);
	glm::mat4 tempWorld = glm::scale(glm::mat4(1), glm::vec3(1.0f / 50.0f)) * objectPtr->toWorld;
	glm::mat4 inverseRotateWorld = glm::transpose(tempWorld);
	float yOffset;
	glm::mat4 scaleMat;

	//cout << "drawing plant with : " << plant4Growth << endl;
	if (plant4Growth - 1 < 2) {
	//	cout << "Inside: " << plant4Growth << endl;
		scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0/8.0f));
		yOffset = 360.0f;
	}
		else if (plant4Growth - 1 == 2) {
			scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0 / 8.0f));
			yOffset = 360.0f;
		}
		else if (plant4Growth - 1 == 3) {
			scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0 / 12.0f));
			yOffset = 567.0f;
		}
		else if (plant4Growth - 1 == 4) {
			scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0 / 16.0f));
			yOffset = 774.0f;
		}
		else if (plant4Growth - 1 == 5) {
			scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0 / 20.0f));
			yOffset = 978.0f;
		}


	//plant4s[0]->draw(shader, tempWorld);

	float rot_on_sphere = 60;
	float tran_on_sphere = 0;

	tempWorld = glm::rotate(glm::mat4(1.0f),
		glm::radians(rot_on_sphere), glm::vec3(1, 0, 0))
		*  inverseRotateWorld;
	tempWorld = glm::transpose(tempWorld) * glm::translate(glm::mat4(1.0f),
		glm::vec3(tran_on_sphere, yOffset - 10, 0));
	plant4s[0]->draw(shader, scaleMat * tempWorld);

	rot_on_sphere = 30;
	tran_on_sphere = -90;

	tempWorld = glm::rotate(glm::mat4(1.0f),
		glm::radians(rot_on_sphere), glm::vec3(1, 0, 0))
		*  inverseRotateWorld;
	tempWorld = glm::transpose(tempWorld) * glm::translate(glm::mat4(1.0f),
		glm::vec3(tran_on_sphere, yOffset - 20, 0));
	plant4s[1]->draw(shader, scaleMat * tempWorld);

	rot_on_sphere = 80;
	tran_on_sphere = -100;

	tempWorld = glm::rotate(glm::mat4(1.0f),
		glm::radians(rot_on_sphere), glm::vec3(1, 0, 0))
		*  inverseRotateWorld;
	tempWorld = glm::transpose(tempWorld) * glm::translate(glm::mat4(1.0f),
		glm::vec3(tran_on_sphere, yOffset - 25, 0));
	plant4s[2]->draw(shader, scaleMat * tempWorld);

	rot_on_sphere = 100;
	tran_on_sphere = 0;

	tempWorld = glm::rotate(glm::mat4(1.0f),
		glm::radians(rot_on_sphere), glm::vec3(1, 0, 0))
		*  inverseRotateWorld;
	tempWorld = glm::transpose(tempWorld) * glm::translate(glm::mat4(1.0f),
		glm::vec3(tran_on_sphere, yOffset - 10, 0));
	plant4s[3]->draw(shader, scaleMat * tempWorld);

	rot_on_sphere = 120;
	tran_on_sphere = 100;

	tempWorld = glm::rotate(glm::mat4(1.0f),
		glm::radians(rot_on_sphere), glm::vec3(1, 0, 0))
		*  inverseRotateWorld;
	tempWorld = glm::transpose(tempWorld) * glm::translate(glm::mat4(1.0f),
		glm::vec3(tran_on_sphere, yOffset - 20, 0));
	plant4s[4]->draw(shader, scaleMat * tempWorld);

	rot_on_sphere = 140;
	tran_on_sphere = -20;

	tempWorld = glm::rotate(glm::mat4(1.0f),
		glm::radians(rot_on_sphere), glm::vec3(1, 0, 0))
		*  inverseRotateWorld;
	tempWorld = glm::transpose(tempWorld) * glm::translate(glm::mat4(1.0f),
		glm::vec3(tran_on_sphere, yOffset - 10, 0));
	plant4s[5]->draw(shader, scaleMat * tempWorld);
}

void Window::initializeFerns(int n) {
	//Ferns style plants setup 
	fern = new Plant(25,10.0f);
	fern->setupRuleSets();

	for (int i = 0; i < n; i++) {
		PlantTransform * p = new PlantTransform("fern ", objectPtr->toWorld);
		p->addChild(fern);
		p->loadPlant(1,0);
		plants.push_back(p);
		ferns.push_back(p);
	}
}

void Window::drawFerns(GLint shader) {
	glUseProgram(shader);
	GLuint uLightPos = glGetUniformLocation(shader, "lightPosInput");
	glUniform3f(uLightPos, sun[0], sun[1], sun[2]);
	glm::mat4 tempWorld = glm::scale(glm::mat4(1), glm::vec3(1.0f / 50.0f)) * objectPtr->toWorld;
	glm::mat4 inverseRotateWorld = glm::transpose(tempWorld);

	float yOffset;
	glm::mat4 scaleMat;

	//cout << "drawing plant with : " << fernGrowth << endl;
	if (fernGrowth - 1 < 2) {
	//	cout << "Inside: " << fernGrowth << endl;
		scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0/8.0f));
		yOffset = 360.0f;
	}
		else if (fernGrowth - 1 == 2) {
			scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0 / 8.0f));
			yOffset = 360.0f;
		}
		else if (fernGrowth - 1 == 3) {
			scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0 / 12.0f));
			yOffset = 567.0f;
		}
		else if (fernGrowth - 1 == 4) {
			scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0 / 16.0f));
			yOffset = 774.0f;
		}
		else if (fernGrowth - 1 == 5) {
			scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0 / 20.0f));
			yOffset = 978.0f;
		}

	float rot_on_sphere = 60;
	float tran_on_sphere = 0;

	tempWorld = glm::rotate(glm::mat4(1.0f),
		glm::radians(rot_on_sphere), glm::vec3(-1, 0, 0))
		*  inverseRotateWorld;
	tempWorld = glm::transpose(tempWorld) * glm::translate(glm::mat4(1.0f),
		glm::vec3(tran_on_sphere, yOffset - 10, 0));
	ferns[0]->draw(shader, scaleMat * tempWorld);

	rot_on_sphere = 30;
	tran_on_sphere = -90;

	tempWorld = glm::rotate(glm::mat4(1.0f),
		glm::radians(rot_on_sphere), glm::vec3(-1, 0, 0))
		*  inverseRotateWorld;
	tempWorld = glm::transpose(tempWorld) * glm::translate(glm::mat4(1.0f),
		glm::vec3(tran_on_sphere, yOffset - 20, 0));
	ferns[1]->draw(shader, scaleMat * tempWorld);

	rot_on_sphere = 80;
	tran_on_sphere = -100;

	tempWorld = glm::rotate(glm::mat4(1.0f),
		glm::radians(rot_on_sphere), glm::vec3(-1, 0, 0))
		*  inverseRotateWorld;
	tempWorld = glm::transpose(tempWorld) * glm::translate(glm::mat4(1.0f),
		glm::vec3(tran_on_sphere, yOffset - 25, 0));
	ferns[2]->draw(shader, scaleMat * tempWorld);

	rot_on_sphere = 100;
	tran_on_sphere = 0;

	tempWorld = glm::rotate(glm::mat4(1.0f),
		glm::radians(rot_on_sphere), glm::vec3(-1, 0, 0))
		*  inverseRotateWorld;
	tempWorld = glm::transpose(tempWorld) * glm::translate(glm::mat4(1.0f),
		glm::vec3(tran_on_sphere, yOffset - 10, 0));
	ferns[3]->draw(shader, scaleMat * tempWorld);

	rot_on_sphere = 120;
	tran_on_sphere = 100;

	tempWorld = glm::rotate(glm::mat4(1.0f),
		glm::radians(rot_on_sphere), glm::vec3(-1, 0, 0))
		*  inverseRotateWorld;
	tempWorld = glm::transpose(tempWorld) * glm::translate(glm::mat4(1.0f),
		glm::vec3(tran_on_sphere, yOffset - 20, 0));
	ferns[4]->draw(shader, scaleMat * tempWorld);

	rot_on_sphere = 140;
	tran_on_sphere = -20;

	tempWorld = glm::rotate(glm::mat4(1.0f),
		glm::radians(rot_on_sphere), glm::vec3(-1, 0, 0))
		*  inverseRotateWorld;
	tempWorld = glm::transpose(tempWorld) * glm::translate(glm::mat4(1.0f),
		glm::vec3(tran_on_sphere, yOffset - 10, 0));
	ferns[5]->draw(shader, scaleMat * tempWorld);
}

void Window::cleanGarden() {
	delete(fern);
	delete(plant4);
	delete(branch);
	for (auto & plant : plants) {
		delete(plant);
	}
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	delete(cube);
	delete(bird);
	delete(planet1);
	delete(bunny2);
	delete(planet2);
	delete(coaster);
	cleanGarden();
	delete(sunSphere);

	delete(cubeSphere);


	glDeleteProgram(shaderProgram);
	glDeleteProgram(toonShaderProgram);
	glDeleteProgram(skyboxShaderProgram);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 8000.0f);
        V = glm::lookAt(cam_pos, cam_look_at, cam_up);
        
	}
}

void Window::idle_callback()
{
	if (travel) {

		if (travelEnd) {
			beltIndex--;
		}
		else {
			beltIndex++;
		}

		if (beltIndex > beltPtr->pathArray.size() - 2) {
			travelEnd = true;
		}
		else if(travelEnd && beltIndex < 40) {
			travelEnd = false;
		}

		cam_pos = beltPtr->pathArray[beltIndex];
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);


	}

	if (ringTravel) {

		if (travelEnd) {
			beltIndex--;
		}
		else {
			beltIndex++;
		}

		if (beltIndex > ringPtr->pathArray.size() - 2) {
			travelEnd = true;
		}
		else if(travelEnd && beltIndex < 40) {
			travelEnd = false;
		}

		cam_pos = ringPtr->pathArray[beltIndex];
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);


	}

	if (birdIndex > birdCoaster.size()) {
		birdIndex = 0;
	}
	bird->toWorld[3] = glm::vec4(birdCoaster[birdIndex]  + glm::vec3(planet1->toWorld[3]) + glm::vec3(-50,0,0),1.0f);
	bird2->toWorld[3] = glm::vec4(birdCoaster[birdIndex] + glm::vec3(planet1->toWorld[3])+ glm::vec3(50,0,0),1.0f);
	bird3->toWorld[3] = glm::vec4(birdCoaster[birdIndex] + glm::vec3(planet1->toWorld[3])+ glm::vec3(0,30,0),1.0f);
	bird4->toWorld[3] = glm::vec4(birdCoaster[birdIndex] + glm::vec3(planet1->toWorld[3])+ glm::vec3(0,-30,0),1.0f);

	birdIndex++;


    //moveCoaster();
    
  //  spiralPtr->update(1.0f);
	beltPtr->update(1.0f);
	ringPtr->update(1.0f);

	objectPtr->update();
	if (objectPtr->angle == 270.0f) {

		if (plant4Growth > 4) {
			plant4Growth = 1;
		}

		plant4s[0]->loadPlant(plant4Growth, 1);

		plant4Growth++;
	}

	else if (objectPtr->angle == 290.0f) {

		if (fernGrowth > 4) {
			fernGrowth = 1;
		}

		ferns[0]->loadPlant(fernGrowth, 0);

		fernGrowth++;

	}

	else if (objectPtr->angle == 340.0f) {

		if (branchGrowth > 4) {
			branchGrowth = 1;
		}

		branches[0]->loadPlant(branchGrowth, 2);

		branchGrowth++;

		procObj->height += 2;
		int h = procObj->height;
		procObj->diamondSquare(0, procObj->size - 1, 0, procObj->size - 1, h); //generate height map
		procObj->generateMesh();
		procObj->loadArray();

	}
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the shader of programID
	glUseProgram(shaderProgram);
	bird->draw(shaderProgram);
	bird2->draw(shaderProgram);
	bird3->draw(shaderProgram);
	bird4->draw(shaderProgram);
    
	glUseProgram(toonShaderProgram);
	GLuint uLightPos = glGetUniformLocation(toonShaderProgram, "lightPosInput");
	glUniform3f(uLightPos, sun[0], sun[1], sun[2]);
    objectPtr->drawSphere(toonShaderProgram);
	planet1->drawSphere(toonShaderProgram);
	planet2->drawSphere(toonShaderProgram);
	if (seeBunny) {
		bunny2->drawSphere(toonShaderProgram);
	}
	drawFerns(toonShaderProgram);
	drawPlant4s(toonShaderProgram);
	drawBranches(toonShaderProgram);

	glEnable(GL_TEXTURE_2D);
	glUseProgram(terrainShader);
	procObj->draw(terrainShader);
	glDisable(GL_TEXTURE_2D);
    
    glUseProgram(skyboxShaderProgram);
    cube->drawSkybox(skyboxShaderProgram);
   
    glUseProgram(particleShaderProgram);
    beltPtr->render(particleShaderProgram);
    ringPtr->render(particleShaderProgram);

   	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
    // Swap buffers
    glfwSwapBuffers(window); //If flash occurs put this line before pollEvents
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //Perform action when key is release
    if(action == GLFW_RELEASE){
        
    }
	// Check for a key press
	if (action == GLFW_PRESS)
	{
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		else if (key == GLFW_KEY_8) {
			plant4->isGridline = !plant4->isGridline;
			branch->isGridline = !branch->isGridline;
			fern->isGridline = !fern->isGridline;
		}
		else if (key == GLFW_KEY_2) {
			tZ = !tZ;
			//cout << glm::to_string(plant4->lastNormal) << endl;
		}
        else if ( key == GLFW_KEY_C && mods & GLFW_MOD_SHIFT)
		{
			seeBunny = false;
			cam_pos = glm::vec3(-409.0f, -185.0f, 409.0f);
			V = glm::lookAt(cam_pos, cam_look_at, cam_up);
		}
        else if ( key == GLFW_KEY_C )
        {
			seeBunny = true;
			cam_pos = glm::vec3(0.0f, 100.0f, 600.0f);		// e  | Position of camera
			V = glm::lookAt(cam_pos, cam_look_at, cam_up);
        }
        else if ( key == GLFW_KEY_P && mods & GLFW_MOD_SHIFT)
		{
		}
        else if ( key == GLFW_KEY_B )
        {
			seeBunny = false;
			travel = !travel;
        }

        else if ( key == GLFW_KEY_R )
        {
			seeBunny = false;
			ringTravel = !ringTravel;
        }
		else if (key == GLFW_KEY_G) {


		if (plant4Growth > 5) {
			plant4Growth = 1;
		}
		if (branchGrowth > 5) {
			branchGrowth = 1;
		}
		if (fernGrowth > 5) {
			fernGrowth = 1;
		}

		cout << plant4Growth << endl;
		plant4s[0]->loadPlant(plant4Growth, 1);
		branches[0]->loadPlant(branchGrowth, 2);
		ferns[0]->loadPlant(fernGrowth, 0);

		plant4Growth++;
		branchGrowth++;
		fernGrowth++;

		}
        else if ( key == GLFW_KEY_H )
        {
			procObj->height += 2;
			int h = procObj->height;
			procObj->diamondSquare(0, procObj->size - 1, 0, procObj->size - 1, h); //generate height map
			procObj->generateMesh();
			procObj->loadArray();

        }

	}
    
}

//Get mouse input
void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    //Get cursor position(x,y)
    //save x,y as prev_position
    if(action == GLFW_RELEASE) {
        objectPtr->rotateMode = false;

    }
    //Detect rotation
    //On mouse click render the selection buffer to draw only spheres
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
		lButtonDown = true;
        prev_position = trackBallMapping(curr_position.x, curr_position.y);

    }
    //Translation in the x-y plane
    //When the right mouse button is pressed and the mouse is moved, move the model in the plane of the screen (similar to what the 'x' and 'y' keys did). Scale this operation so that when the model is in the screen space z=0 plane, the model follows your mouse pointer as closely as possible.
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
		lButtonDown = false;
        prev_position = trackBallMapping(curr_position.x, curr_position.y);
        objectPtr->rotateMode = true;
    }
	else {
		lButtonDown = false;
	}
}

// cursor_position_callback() will be automatically called whenever
//mouse moves (as defined in main), so we never need to call it in our code.
void Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos){
	if (lButtonDown) {
		double x_trans = xpos - prevPos.x;
		double y_trans = prevPos.y - ypos;
		sunSphere->toWorld = sunSphere->toWorld * glm::translate(glm::mat4(1.0f),glm::vec3(x_trans/50.0f,y_trans/50.0f,0.0f));
		sun = sunSphere->toWorld[3];
	}
	prevPos.x = xpos;
	prevPos.y = ypos;

    glm::vec3 direction;
    
    //Store cursor position in global variable
    glfwGetCursorPos(window, &curr_x, &curr_y);
    
    //Get curser position(x,y)
    curr_position = trackBallMapping(xpos, ypos);
    
    direction = curr_position - prev_position;
    
    float vel = direction.length();
    
    /**You calculate rotation axis and angle w.r.t. old position and current position BEFORE you update old position to current position.
     * use cross product of prev pos and curr pos a = v x w
     */
    glm::vec3 rotationAxis = glm::cross(prev_position, curr_position);
    
    /** rotate angle glm::dot and acos should give a pretty accurate answer.
     * You should be getting results in the range [0, pi/2],
     * since glm::dot return a value in the range [0,1] if your vectors are normalized.
     */
    auto rotationAngle_dot = glm::dot(prev_position, curr_position);
    rotationAngle_dot = glm::clamp(rotationAngle_dot, 0.0f, 1.0f);
    float rotationAngle = glm::acos(rotationAngle_dot);

    
    //cout << translateX << " Translate of X" << endl;
    //cout << translateY << " Translate of Y" << endl;

    
    /** In cursor_position_callback(), after finding out the angle and axis and updating old cursor position,
      * all we do is to check flags (ROTATE, TRANSLATE etc) and call the corresponding 
      * OBJObject functions (such as obj->orbit()).
      * Once you set the flags, make sure the flags are used in cursor_position_callback()
      * to do either rotation or translation.
      */
    if(objectPtr->rotateMode == true ) {
        if(vel > 0.0001){
//            objectPtr->rotate(rotationAxis, rotationAngle);
			/*
			cout << "SPHERE: "<< glm::to_string(objectPtr->toWorld) << endl;
			cout << glm::to_string(pt1->M) << endl;
            */
            //rotate camera
            rotateCamera(rotationAxis, rotationAngle);
        }
        prev_position = curr_position;
    }
    curr_position.x = curr_x;
    curr_position.y = curr_y;
}


//Translation along the z-axis
//Use the mouse wheel to move the model along the screen space z axis (i.e., in and out of the screen = push back and pull closer).
void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset){

    glm::mat4 scaleM;
    glm::mat4 transM;

	if (tZ) {


		if (yoffset < 0) {
			//scale object
	//        objectPtr->translateZ(true);
		sunSphere->translateZ(true);
		sun = sunSphere->toWorld[3];
	//	cout << glm::to_string(sun) << endl;

		}
		else {

			//scale object
	//        objectPtr->translateZ(false);
		sunSphere->translateZ(false);
		sun = sunSphere->toWorld[3];
//		cout << glm::to_string(sun) << endl;

		}
	}
	else 
	Window::V = Window::V * glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, yoffset / 5.0f));

    
//    glm::vec3 cameraPos = cam_pos + glm::vec3(transM[3]);
//    
//    //update camera position
//    cam_pos = cameraPos;
//    
//    // recalculate V the view matrix
//    V = glm::lookAt(cam_pos, cam_look_at, cam_up);
}

glm::vec3 Window::trackBallMapping(float x, float y){
    glm::vec3 v;    // Vector v is the synthesized 3D position of the mouse location on the trackball
    float d;     // this is the depth of the mouse location: the delta between the plane through the center of the trackball and the z position of the mouse
    v.x = (2.0*x - width) / width;   // this calculates the mouse X position in trackball coordinates, which range from -1 to +1
    v.y = (height - 2.0*y) / height;   // this does the equivalent to the above for the mouse Y position
    v.z = 0.0;   // initially the mouse z position is set to zero, but this will change below
    d = glm::length(v);    // this is the distance from the trackball's origin to the mouse location, without considering depth (=in the plane of the trackball's origin)
    d = (d<1.0) ? d : 1.0;   // this limits d to values of 1.0 or less to avoid square roots of negative values in the following line
    v.z = sqrtf(1.001 - d*d);  // this calculates the Z coordinate of the mouse position on the trackball, based on Pythagoras: v.z*v.z + d*d = 1*1
    glm::normalize(v); // Still need to normalize, since we only capped d, not v.
   
    return v;  // return the mouse location on the surface of the trackball
}
    
  
void Window::rotateCamera(glm::vec3 rot_axis, float rot_angle){
    
    glm::mat4 R = glm::rotate(glm::mat4(), rot_angle, rot_axis);
    
    glm::vec3 cameraPos = cam_pos * glm::mat3(R);
    //apply rotation
    // camPos = camPos * R;

    //update camera position
    cam_pos = cameraPos;
    
    // recalculate V the view matrix
    V = glm::lookAt(cam_pos, cam_look_at, cam_up);
    
}