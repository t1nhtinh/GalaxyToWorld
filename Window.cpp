#include "Window.h"
#include "Node.h"
#include "MatrixTransform.h"
#include "Geode.h"
#include <time.h>

const char* window_title = "GLFW Starter Project";
Cube * cube;
GLint shaderProgram, skyboxShaderProgram, curveShaderProgram, selectionShaderProgram;


OBJObject * objectPtr;
OBJObject * bunny;
OBJObject * dragon;
OBJObject * bear;
OBJObject * coaster;

MatrixTransform * rollerCoaster; // roller coaster of bezier curves
MatrixTransform * bezPtr; //single cubic bezier curve
MatrixTransform * curveMtxPtr; //bezier curve matrix
MatrixTransform * sphereMtxPtr; //family of sphere matrices
MatrixTransform * selectionPtr; //family of control points
MatrixTransform * coasterMtx;

Geode * curvePtr;
Geode * anchorPtr;
Geode * approxControlPtr;
//Geode * handlePtr;
Geode * coasterPtr;

BezierCurve * handlePtr;

// On some systems you need to change this to the absolute path
//#define VERTEX_SHADER_PATH "../shader.vert"
//#define FRAGMENT_SHADER_PATH "../shader.frag"

#define VERTEX_SHADER_PATH "/Users/tinhdang/Documents/UCSD/CSE167_IntroToComputerGraphics/GalaxyToWorld/GalaxyToWorld/shader.vert"
#define FRAGMENT_SHADER_PATH "/Users/tinhdang/Documents/UCSD/CSE167_IntroToComputerGraphics/GalaxyToWorld/GalaxyToWorld/shader.frag"
#define VERTEX_SKYBOXSHADER "/Users/tinhdang/Documents/UCSD/CSE167_IntroToComputerGraphics/GalaxyToWorld/GalaxyToWorld/skyboxShader.vert"
#define FRAGMENT_SKYBOXSHADER "/Users/tinhdang/Documents/UCSD/CSE167_IntroToComputerGraphics/GalaxyToWorld/GalaxyToWorld/skyboxShader.frag"

#define VERTEX_CURVESHADER "/Users/tinhdang/Documents/UCSD/CSE167_IntroToComputerGraphics/GalaxyToWorld/GalaxyToWorld/curve.vert"
#define FRAGMENT_CURVESHADER "/Users/tinhdang/Documents/UCSD/CSE167_IntroToComputerGraphics/GalaxyToWorld/GalaxyToWorld/curve.frag"

#define VERTEX_SELECTIONSHADER "/Users/tinhdang/Documents/UCSD/CSE167_IntroToComputerGraphics/GalaxyToWorld/GalaxyToWorld/selection.vert"
#define FRAGMENT_SELECTIONSHADER "/Users/tinhdang/Documents/UCSD/CSE167_IntroToComputerGraphics/GalaxyToWorld/GalaxyToWorld/selection.frag"

//Lighting
Light * light_ptr;
Light * dir_light;
Light * point_light;
Light * spot_light;

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

vector<glm::vec3> pointsArray;
vector<glm::vec3> handlePointsArray;
vector<vector<glm::vec3>> pathArray;
vector<float> lengthOfCurve;
vector<BezierCurve*> bezierCurveArray;
vector<OBJObject*> sphereArray;
vector<MatrixTransform*> sphereMtxArray;


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
void Window::initialize_objects()
{
    dir_light = new Light(0);
    point_light = new Light(1);
    spot_light = new Light(2);
    
	cube = new Cube();
    
	// Load the shader program. Make sure you have the correct filepath up top
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
    skyboxShaderProgram = LoadShaders(VERTEX_SKYBOXSHADER, FRAGMENT_SKYBOXSHADER);
    curveShaderProgram = LoadShaders(VERTEX_CURVESHADER, FRAGMENT_CURVESHADER);
    selectionShaderProgram = LoadShaders(VERTEX_SELECTIONSHADER, FRAGMENT_SELECTIONSHADER);
    
    
    light_ptr = dir_light;
    light_ptr->lightMode = 0;
   // objectPtr->matMode = 0;
    
    //Initialize control points and store in an array
    initializePointsArray();
    
    
    //Initialize handle points after populating points array
    handlePtr = new BezierCurve(glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f));
    handlePointsArray = handlePtr->initHandlePoints(pointsArray);
    
    //Initalize the rollerCoaster Matrix Transform to store the family of Bezier Curves
    rollerCoaster = new MatrixTransform(glm::mat4(1.0f));
    
    
    //Construct sphere for anchor control points and approximating control points
    anchorPtr = new Geode();
    anchorPtr->spherePtr = new OBJObject("/Users/tinhdang/Documents/UCSD/CSE167_IntroToComputerGraphics/GalaxyToWorld/GalaxyToWorld/sphere.obj");
    anchorPtr->isSphere = true;
    anchorPtr->spherePtr->matMode = 1; //Color control/anchor points red
    
    approxControlPtr = new Geode();
    approxControlPtr->spherePtr = new OBJObject("/Users/tinhdang/Documents/UCSD/CSE167_IntroToComputerGraphics/GalaxyToWorld/GalaxyToWorld/sphere.obj");
    approxControlPtr->isSphere = true;
    approxControlPtr->spherePtr->matMode = 2; //Color approximating control points green
    
    int P0 = 0;
    int P1 = 1;
    int P2 = 2;
    int P3 = 3;
    
    //Create a matrix transform to draw spheres in back buffer
    selectionPtr = new MatrixTransform(glm::mat4(1.0f));
    
    
    /** Scene Graph to display rollar coast **/
    for(int i = 0; i < 8; i++){
        /** Initialize roller coaster using scene graph **/
        bezPtr = new MatrixTransform(glm::mat4(1.0f));

        curvePtr = new Geode(); //initialize Geode object
        
        //Initialize bezier curve within Geode object
        curvePtr->bezierCurve = new BezierCurve(pointsArray[P0], pointsArray[P1], pointsArray[P2], pointsArray[P3]);
        bezierCurveArray.push_back(curvePtr->bezierCurve);
        lengthOfCurve.push_back(curvePtr->bezierCurve->length);
        
        curvePtr->isCurve = true; 
        bezPtr->addChild(curvePtr); //add curve object as child of Matrix Transform
        

        /** 1. Position spheres at control points
          * 2. add sphere Geode as child to sphere matrix
          * 3. add sphere Matrix Transform as child to bezier curve matrix
          * 4. For selection buffer, store a scene graph containing on the control points to be drawn in the back buff
          *     a. Associate an ID for each control point sphere
          */
        sphereMtxPtr = new MatrixTransform( curvePtr->bezierCurve->toWorld *  glm::translate(glm::mat4(1.0f), curvePtr->bezierCurve->P0));
        sphereMtxPtr->addChild(anchorPtr);
        bezPtr->addChild(sphereMtxPtr);
        selectionPtr->addChild(sphereMtxPtr);
        sphereMtxArray.push_back(sphereMtxPtr);
        
        
    
        sphereMtxPtr = new MatrixTransform( curvePtr->bezierCurve->toWorld *  glm::translate(glm::mat4(1.0f), curvePtr->bezierCurve->P1));
        sphereMtxPtr->addChild(approxControlPtr);
        bezPtr->addChild(sphereMtxPtr);
        selectionPtr->addChild(sphereMtxPtr);
        sphereMtxArray.push_back(sphereMtxPtr);

      
        
        sphereMtxPtr = new MatrixTransform( curvePtr->bezierCurve->toWorld *  glm::translate(glm::mat4(1.0f), curvePtr->bezierCurve->P2));
        sphereMtxPtr->addChild(approxControlPtr);
        bezPtr->addChild(sphereMtxPtr);
        selectionPtr->addChild(sphereMtxPtr);
        sphereMtxArray.push_back(sphereMtxPtr);

        
        sphereMtxPtr = new MatrixTransform( curvePtr->bezierCurve->toWorld *  glm::translate(glm::mat4(1.0f), curvePtr->bezierCurve->P3));
        sphereMtxPtr->addChild(anchorPtr);
        bezPtr->addChild(sphereMtxPtr);
        selectionPtr->addChild(sphereMtxPtr);
        sphereMtxArray.push_back(sphereMtxPtr);

      
        // Add childs to Family of bezier curves and control points
        rollerCoaster->addChild(bezPtr);
        
        P0 += 4;
        P1 += 4;
        P2 += 4;
        P3 += 4;
        
        pathArray.push_back(curvePtr->bezierCurve->vertices); //Grab the path
    
    }

    //iterate through path and store max height
    for(int j = 0; j < pathArray.size(); j++){
        for (int k = 0; k < pathArray[j].size(); k++){
            if(maxH < pathArray[j][k].y){
                maxH = pathArray[j][k].y;
                start = pathArray[j][k];
                
                //Set the starting position of the roller coaster at highest point
                startCurve = curveCount = j;
                startLine = lineCount = k;
            }
        }
    }
    
    //Create the roller coaster using a sphere
    coaster = new OBJObject ("/Users/tinhdang/Documents/UCSD/CSE167_IntroToComputerGraphics/GalaxyToWorld/GalaxyToWorld/sphere.obj");
    
    glm::mat4 scaleCoaster = glm::scale((glm::mat4(1.0f), glm::vec3(5.0f)));
    coaster->toWorld = scaleCoaster * coaster->toWorld;
    coaster->toWorld[3] = glm::vec4(start, 1.0f);
    
    objectPtr = coaster;
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	delete(cube);
    delete(dir_light);
    delete(spot_light);
    delete(point_light);
    
	glDeleteProgram(shaderProgram);
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
}

void Window::display_callback(GLFWwindow* window)
{
    
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
  
    // Use the shader of programID
	//glUseProgram(shaderProgram);
    
    //objectPtr->drawSpere(shaderProgram);
    
    
//    light_ptr->draw(shaderProgram);
    // Render the cube
//	//cube->draw(shaderProgram);
    glUseProgram(skyboxShaderProgram);
    
    cube->drawSkybox(skyboxShaderProgram);
   
    GLuint camPos = glGetUniformLocation(skyboxShaderProgram, "cameraPos");
    glUniform3fv(camPos, 1, &cam_pos[0]);
    
    coaster->draw(skyboxShaderProgram);

    
    //Draw roller Coaster
   // glUseProgram(curveShaderProgram);
    //rollerCoaster->draw(curveShaderProgram, glm::mat4(1.0f));

    //Draw handles points
   // handlePtr->draw(curveShaderProgram, glm::mat4(1.0f));
    //handlePtr->drawCurve(curveShaderProgram);

    // Swap buffers
    glfwSwapBuffers(window); //If flash occurs put this line before pollEvents

   	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
//	// Swap buffers
//	glfwSwapBuffers(window); //If flash occurs put this line before pollEvents
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
        else if ( key == GLFW_KEY_P && mods & GLFW_MOD_SHIFT)
        {
            coaster->translateMode = false;
        }
        else if ( key == GLFW_KEY_P )
        {
            coaster->translateMode = true;
        }
        else if (key == GLFW_KEY_S)
        {
            coaster->toWorld[3] = glm::vec4(start, 1.0f);
            //Ensure that sphere follows path from starting from highestpoint
            curveCount = startCurve;
            lineCount = startLine;
            
            //iterate through path and store max height
            for(int m = 0; m < pathArray.size(); m++){
                for (int n = 0; n < pathArray[m].size(); n++){
                    if(maxH < pathArray[m][n].y){
                        maxH = pathArray[m][n].y;
                        start = pathArray[m][n];
                        
                        //Set the starting position of the roller coaster at highest point
                        startCurve = curveCount = m;
                        startLine = lineCount = n;
                    }
                }
            }

        }
        else if ( key == GLFW_KEY_F && mods & GLFW_MOD_SHIFT)
        {
            coaster->physiceMode = false;
        }
        else if (key == GLFW_KEY_F)
        {
            coaster->physiceMode = true;
        
        }
    


	}
    
}


//Get mouse input
void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    //Get cursor position(x,y)
    //save x,y as prev_position
    if(action == GLFW_RELEASE) {
        objectPtr->rotateMode = false;
        selectionPtr->selectionMode = false;

    }
    //Detect rotation
    //On mouse click render the selection buffer to draw only spheres
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
    
        prev_position.x = curr_x;
        prev_position.y = curr_y;
        
        //Clear the scene to draw the control points in the back buffer
         glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
         glUseProgram(selectionShaderProgram);
         selectionPtr->selectionMode = true;
        
        //Draw the control points
        selectionPtr->draw(selectionShaderProgram, glm::mat4(1.0f));
    
        //Initilize an array to store pixel components
        unsigned char pix[4];
        
        //Read pixel at cursor position
        glReadPixels((GLint)curr_x*2, (GLint)(height - curr_y*2), 1,1,GL_RGBA, GL_UNSIGNED_BYTE, &pix);
            
        id = (int)pix[0] - 1;
        
    
        
        cout << id << " sphere selected" << endl;
 
    }
    //Translation in the x-y plane
    //When the right mouse button is pressed and the mouse is moved, move the model in the plane of the screen (similar to what the 'x' and 'y' keys did). Scale this operation so that when the model is in the screen space z=0 plane, the model follows your mouse pointer as closely as possible.
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
        
        prev_position = trackBallMapping(curr_position.x, curr_position.y);
        objectPtr->rotateMode = true;
       
    }
    
}

// cursor_position_callback() will be automatically called whenever
//mouse moves (as defined in main), so we never need to call it in our code.
void Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos){
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
            //objectPtr->rotate(rotationAxis, rotationAngle);
            
            //rotate camera
            rotateCamera(rotationAxis, rotationAngle);
        }
        prev_position = curr_position;
    }
    //cout << curr_x << " " << curr_y << endl;
    curr_position.x = curr_x;
    curr_position.y = curr_y;
    
    auto translate_distance = curr_position - prev_position;
    auto translateX = translate_distance.x;
    auto translateY = translate_distance.y;
    
    if(selectionPtr->selectionMode == true){
        
        if(id >= 0 ){
            int dup_0 = id/4;
            int dup_1 = id % 4;
            
            //    P3 (or Q0) = (P2 + Q1) / 2
            //    If P3/Q0 gets moved, move P2 and Q1 the same amount
            //    If P2 gets moved, keep P3/Q0 in place and move Q1 by the negative amount
            //    Do the same thing to P2 if Q1 gets moved

            if(id == 0 || id == 31){
                pointsArray[0].x = pointsArray[0].x - translateX;
                pointsArray[0].y = pointsArray[0].y + translateY;
                
                pointsArray[31].x = pointsArray[31].x - translateX;
                pointsArray[31].y = pointsArray[31].y + translateY;
                
                pointsArray[30].x = pointsArray[30].x - translateX;
                pointsArray[30].y = pointsArray[30].y + translateY;
                
                pointsArray[1].x = pointsArray[1].x - translateX;
                pointsArray[1].y = pointsArray[1].y + translateY;

                
            }
            else if(id % 4 == 0 ){
                pointsArray[id].x = pointsArray[id].x - translateX;
                pointsArray[id].y = pointsArray[id].y + translateY;
                
                pointsArray[id - 1].x = pointsArray[id - 1].x - translateX;
                pointsArray[id - 1].y = pointsArray[id - 1].y + translateY;
                
              

            }
            else if ((id + 1) % 4 == 0){
                pointsArray[id].x = pointsArray[id].x - translateX;
                pointsArray[id].y = pointsArray[id].y + translateY;
                
                pointsArray[id + 1].x = pointsArray[id + 1].x - translateX;
                pointsArray[id + 1].y = pointsArray[id + 1].y + translateY;

            }
            else {
            
                pointsArray[id].x = pointsArray[id].x - translateX;
                pointsArray[id].y = pointsArray[id].y + translateY;
            }
            
            cout << id << " idddddd" << endl;
            //cout << translateY << " Translate of Y" << endl;
            // cout << translateX << " Translate of X" << endl;

           // cout << pointsArray[id].x << " " << pointsArray[id].y << endl;
            
            int P0 = 0;
            int P1 = 1;
            int P2 = 2;
            int P3 = 3;

            pathArray.clear();
            for(int k = 0; k < 8; k++){
                bezierCurveArray[k]->computeVertices(pointsArray[P0], pointsArray[P1],pointsArray[P2], pointsArray[P3]);
                sphereMtxArray[P0]->MT[3] = glm::vec4(pointsArray[P0], 1.0f);
                sphereMtxArray[P1]->MT[3] = glm::vec4(pointsArray[P1], 1.0f);
                sphereMtxArray[P2]->MT[3] = glm::vec4(pointsArray[P2], 1.0f);
                sphereMtxArray[P3]->MT[3] = glm::vec4(pointsArray[P3], 1.0f);

                P0 += 4;
                P1 += 4;
                P2 += 4;
                P3 += 4;
                
                pathArray.push_back(bezierCurveArray[k]->vertices);
            }
            
            
            handlePtr->initHandlePoints(pointsArray);
        }
        
        prev_position = curr_position;
    }
    
    //set prev_pos = curr_pos
    ///prev_position = curr_position;

}


//Translation along the z-axis
//Use the mouse wheel to move the model along the screen space z axis (i.e., in and out of the screen = push back and pull closer).
void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset){

    glm::mat4 scaleM;
    glm::mat4 transM;

    if( yoffset < 0 ) {
        //scale object
        objectPtr->translateZ(true);
        
    }
    else {
        
        //scale object
        objectPtr->translateZ(false);
        
    }
    
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

void Window::moveCoaster(){
    glm::vec3 position;
    
    //Move the Roller Coaster around the track
    if(coaster->translateMode == true){
        
        
        //Apply PHYSICS
        if (coaster->physiceMode == true) {
            
            //update current height of sphere
            currH = coaster->toWorld[3][1];
            //Compute deltaH: should always be negative except at highest point where deltaH = 0
            deltaH = currH - maxH;
            
            //velocity formula
            velocity = sqrt(-2.0 * a * deltaH) + c;
            cout << velocity << ".. The velocity" << endl;
            //store control point matrix of curve
            
            glm::mat4 controlPts = bezierCurveArray[curveCount]->pointMatrix;
            
            float len = lengthOfCurve[curveCount];
            
            float t = (lineCount)/ 150.0;
            
            //Apply physics in Bezier curve space: t + v
            float physics = t + (velocity / len); //t + v
            
            lineCount++; //increment to next line
            
            //If sphere has moved through 150 lines, reset lines to 0 and move on to next curve
            //If (t + v) > 1.0 move on to next curve. Simply subtract 1 from t
            if(curveCount == 8){
                curveCount = 0;
            }
            
            if(lineCount == 150 || physics > 1.0){
                lineCount = 0;
                curveCount++;
                
                if(curveCount == 8){
                    curveCount = 0;
                }
            }
            else {
                
                position = handlePtr->createCubicBezierCurve(controlPts, physics);
                
                //Modify position of sphere
                coaster->toWorld[3] = glm::vec4(position, 1.0f);
            }
            
        }
        //Move coaster without physics
        else
        {
            //Modify position of sphere
            coaster->toWorld[3] = glm::vec4(pathArray[curveCount][lineCount], 1.0f);
            lineCount++;
            
            //cout << lineCount << "determine which line it has reached" << endl;
            if(lineCount == 150){
                lineCount = 0;
                curveCount++;
                
                if(curveCount == 8){
                    curveCount = 0;
                }
            }
        }
    }

}

void Window::initializePointsArray(){
    
    glm::vec3 point0(-70.0f, -70.0f, 60.0f);
    glm::vec3 point1(-50.0f, -80.0f, 60.0f);
    glm::vec3 point2(-10.0f, -25.0f, 40.0f);
    glm::vec3 point3(0.0f, -10.0f, 40.0f);
    pointsArray.push_back(point0);
    pointsArray.push_back(point1);
    pointsArray.push_back(point2);
    pointsArray.push_back(point3);
    
    point0 =  glm::vec3(0.0f, -10.0f, 40.0f);
    point1 = glm::vec3(5.0f, -5.0f, 50.0f);
    point2 = glm::vec3(15.0f, -70.0f, 60.0f);
    point3 = glm::vec3(40.0f, -60.0f, 40.0f);
    pointsArray.push_back(point0);
    pointsArray.push_back(point1);
    pointsArray.push_back(point2);
    pointsArray.push_back(point3);
    
    point0 =  glm::vec3(40.0f, -60.0f, 40.0f);
    point1 = glm::vec3(80.0f, -50.0f, 20.0f);
    point2 = glm::vec3(60.0f, 50.0f, -30.0f);
    point3 = glm::vec3(45.0f, 45.0f, -20.0f);
    pointsArray.push_back(point0);
    pointsArray.push_back(point1);
    pointsArray.push_back(point2);
    pointsArray.push_back(point3);

    point0 =  glm::vec3(45.0f, 45.0f, -20.0f);
    point1 = glm::vec3(35.0f, 40.0f, -10.0f);
    point2 = glm::vec3(40.0f, 0.0f, -10.0f);
    point3 = glm::vec3(20.0f, 5.0f, 0.0f);
    pointsArray.push_back(point0);
    pointsArray.push_back(point1);
    pointsArray.push_back(point2);
    pointsArray.push_back(point3);

    point0 =  glm::vec3(20.0f, 5.0f, 0.0f);
    point1 = glm::vec3(5.0f, 5.0f, 0.0f);
    point2 = glm::vec3(10.0f, 10.0f, -20.0f);
    point3 = glm::vec3(0.0f, 15.0f, -10.0f);
    pointsArray.push_back(point0);
    pointsArray.push_back(point1);
    pointsArray.push_back(point2);
    pointsArray.push_back(point3);
    
    point0 =  glm::vec3(0.0f, 15.0f, -10.0f);
    point1 = glm::vec3(-20.0f, 20.0f, -5.0f);
    point2 = glm::vec3(-10.0f, 5.0f, 0.0f);
    point3 = glm::vec3(-25.0f, 10.0f, -5.0f);
    pointsArray.push_back(point0);
    pointsArray.push_back(point1);
    pointsArray.push_back(point2);
    pointsArray.push_back(point3);
    
    point0 =  glm::vec3(-25.0f, 10.0f, -5.0f);
    point1 = glm::vec3(-45.0f, 25.0f, -10.0f);
    point2 = glm::vec3(-30.0f, 80.0f, -20.0f);
    point3 = glm::vec3(-60.0f, 90.0f, -40.0f);
    pointsArray.push_back(point0);
    pointsArray.push_back(point1);
    pointsArray.push_back(point2);
    pointsArray.push_back(point3);
    
    point0 =  glm::vec3(-60.0f, 90.0f, -40.0f);
    point1 = glm::vec3(-80.0f, 100.0f, 0.0f);
    point2 = glm::vec3(-100.0f, -60.0f, 40.0f);
    point3 = glm::vec3(-70.0f, -70.0f, 60.0f);
    pointsArray.push_back(point0);
    pointsArray.push_back(point1);
    pointsArray.push_back(point2);
    pointsArray.push_back(point3);

    
}


