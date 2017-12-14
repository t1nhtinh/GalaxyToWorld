#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <iostream>

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
// If modern OpenGL replace gl.h with gl3.h
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/glu.h>
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include "Cube.h"
#include "shader.h"
#include "OBJObject.h"
#include "Light.h"
#include "BezierCurve.h"
#include <vector>
#include "Particle.h"


class Window
{
public:
	static int width;
	static int height;
	static glm::mat4 P; // P for projection
	static glm::mat4 V; // V for view
	static void initialize_objects();
	static void clean_up();
	static GLFWwindow* create_window(int width, int height);
	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    
    //Get mouse input
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    //Get cursor position
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    //scroll call back function
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static glm::vec3 trackBallMapping(float x, float y);
    
    
    double xoffset, yoffset;
    
    static void rotateCamera(glm::vec3, float);
    static void initializePointsArray();
    
    static void moveCoaster(); 
    
};

#endif
