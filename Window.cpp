#include "Window.h"
#include "Node.h"
#include "MatrixTransform.h"
#include "Plant.h"
#include "PlantTransform.h"
#include "Geode.h"
#include <time.h>

const char* window_title = "GLFW Starter Project";
Cube * cube;
GLint shaderProgram, skyboxShaderProgram, selectionShaderProgram,toonShaderProgram;

OBJObject * objectPtr;
OBJObject * bunny;
OBJObject * dragon;
OBJObject * bear;
OBJObject * coaster;
OBJObject * sunSphere;

#define VERTEX_SHADER_PATH "../shader.vert"
#define FRAGMENT_SHADER_PATH "../shader.frag"
#define VERTEX_SKYBOXSHADER "../skyboxShader.vert"
#define FRAGMENT_SKYBOXSHADER "../skyboxShader.frag"

#define VERTEX_TOONSHADER "../environmentshader.vert"
#define FRAGMENT_TOONSHADER "../environmentshader.frag"

#define VERTEX_SELECTIONSHADER "../selection.vert"
#define FRAGMENT_SELECTIONSHADER "../selection.frag"

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 200.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is
glm::vec3 prev_position;
glm::vec3 curr_position;
double curr_x, curr_y;

int Window::width;
int Window::height;

glm::mat4 Window::P;
glm::mat4 Window::V;

int startCurve, startLine;

static float a = 0.00005f; //acceleration
static float c = 0.005f; //some constant to push the sphere
float velocity;
float maxH = 0.0; //max height
glm::vec3 start; //position coaster at the highest point
float deltaH = 0.0; //max height of track - current height of sphere
float currH = 0.0;
int id;


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

void Window::initialize_objects()
{
	lButtonDown = false;

	cube = new Cube();
    
	// Load the shader program. Make sure you have the correct filepath up top
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
    skyboxShaderProgram = LoadShaders(VERTEX_SKYBOXSHADER, FRAGMENT_SKYBOXSHADER);
	toonShaderProgram = LoadShaders(VERTEX_TOONSHADER, FRAGMENT_TOONSHADER);
    selectionShaderProgram = LoadShaders(VERTEX_SELECTIONSHADER, FRAGMENT_SELECTIONSHADER);
    
    //Create the roller coaster using a sphere
    coaster = new OBJObject ("sphere.obj");
    
    glm::mat4 scaleCoaster = glm::scale((glm::mat4(1.0f), glm::vec3(50.0f)));
    coaster->toWorld = scaleCoaster * coaster->toWorld;
    coaster->toWorld[3] = glm::vec4(cam_look_at, 1.0f);

    objectPtr = coaster;
    objectPtr->matMode = 0;

	initializeFerns(3);
	fernGrowth = 1;
	initializePlant4s(3);
	plant4Growth = 1;
	initializeBranch(3);
	branchGrowth = 1;

	//Initialize the sun
	sunSphere = new OBJObject("sphere.obj");
	sun = glm::vec3(glm::vec3(-25, 13, 150));
	sunSphere->toWorld[3] = glm::vec4(sun,1.0f);
	tZ = false;// switch for control over camera or sun
}

void Window::initializeBranch(int n) {
	//Branch style plants setup 
	branch = new Plant(5,2.0f);

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

	float rot_on_sphere = 0;
	float tran_on_sphere = 0;
	for (int i = 0; i < branches.size(); i++) {
		rot_on_sphere += 5.0f;
		tran_on_sphere += 5.0f;
		if (rot_on_sphere > 90.0f) {
			rot_on_sphere = 0.0f;
		}

		tempWorld = glm::rotate(glm::mat4(1.0f), 
			glm::radians(rot_on_sphere), glm::vec3(0, 0, 1))
			*  inverseRotateWorld;
		tempWorld = glm::transpose(tempWorld) * glm::translate(glm::mat4(1.0f),
			glm::vec3(tran_on_sphere,0,0));
		branches[i]->draw(shader, tempWorld);
	}
}

void Window::initializePlant4s(int n) {
	//Plant 4 style plants setup 
	plant4 = new Plant(5,2.0f);

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

	//plant4s[0]->draw(shader, tempWorld);

	float rot_on_sphere = 0;
	float tran_on_sphere = 0;
	for (int i = 0; i < plant4s.size(); i++) {
		rot_on_sphere += 5.0f;
		tran_on_sphere += 0.2f;
		if (rot_on_sphere > 90.0f) {
			rot_on_sphere = 0.0f;
		}

		tempWorld = glm::rotate(glm::mat4(1.0f), 
			glm::radians(rot_on_sphere), glm::vec3(1, 0, 0))
			*  inverseRotateWorld;
		tempWorld = glm::transpose(tempWorld) * glm::translate(glm::mat4(1.0f),
			glm::vec3(tran_on_sphere,0,0));
		plant4s[i]->draw(shader, tempWorld);
	}
}

void Window::initializeFerns(int n) {
	//Ferns style plants setup 
	fern = new Plant(25,2.0f);
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

	float rot_on_sphere = 0;
	float tran_on_sphere = 0;
	for (int i = 0; i < ferns.size(); i++) {
		rot_on_sphere += 5.0f;
		tran_on_sphere += 0.1f;
		if (rot_on_sphere > 90.0f) {
			rot_on_sphere = 0.0f;
		}

		tempWorld = glm::rotate(glm::mat4(1.0f), 
			glm::radians(rot_on_sphere), glm::vec3(-1, 0, 0))
			*  inverseRotateWorld;
		tempWorld = glm::transpose(tempWorld) * glm::translate(glm::mat4(1.0f),
			glm::vec3(tran_on_sphere,0,0));
		ferns[i]->draw(shader, tempWorld);
	}
		
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
  delete(coaster);
  cleanGarden();
  delete(sunSphere);

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
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 3000.0f);
        V = glm::lookAt(cam_pos, cam_look_at, cam_up);
        
	}
}

void Window::idle_callback()
{
    //moveCoaster();
	objectPtr->update();
	if (plant4Growth > 4) {
		plant4Growth = 1;
	}
	if (branchGrowth > 4) {
		branchGrowth = 1;
	}
	if (fernGrowth > 4) {
		fernGrowth = 1;
	}
	if (objectPtr->angle == 270.0f) {
		plant4s[0]->loadPlant(plant4Growth, 1);
		branches[0]->loadPlant(branchGrowth, 2);
		ferns[0]->loadPlant(fernGrowth, 0);

		plant4Growth++;
		branchGrowth++;
		fernGrowth++;
	}
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the shader of programID
    
	glUseProgram(toonShaderProgram);
	GLuint uLightPos = glGetUniformLocation(toonShaderProgram, "lightPosInput");
	glUniform3f(uLightPos, sun[0], sun[1], sun[2]);
    objectPtr->drawSpere(toonShaderProgram);
	sunSphere->drawSpere(toonShaderProgram);
    
    glUseProgram(skyboxShaderProgram);
    
    cube->drawSkybox(skyboxShaderProgram);
   
	drawFerns(toonShaderProgram);
	drawPlant4s(toonShaderProgram);
	drawBranches(toonShaderProgram);

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
        else if ( key == GLFW_KEY_P && mods & GLFW_MOD_SHIFT)
        {
            coaster->translateMode = false;
			coaster->translateX(5);
        }
        else if ( key == GLFW_KEY_P )
        {
            coaster->translateMode = true;
			coaster->translateX(-5);
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
            objectPtr->rotate(rotationAxis, rotationAngle);
			/*
			cout << "SPHERE: "<< glm::to_string(objectPtr->toWorld) << endl;
			cout << glm::to_string(pt1->M) << endl;
            */
            //rotate camera
//            rotateCamera(rotationAxis, rotationAngle);
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