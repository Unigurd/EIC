/*
* Copyright 2019 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/


#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "Utils.h"


/* --------------------------------------------- */
// Prototypes
/* --------------------------------------------- */



/* --------------------------------------------- */
// Global variables
/* --------------------------------------------- */


/* --------------------------------------------- */
// Callbacks
/* --------------------------------------------- */


// To be changed
void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}


/* --------------------------------------------- */
// Main
/* --------------------------------------------- */

int main(int argc, char** argv)
{
	/* --------------------------------------------- */
	// Load settings.ini
	/* --------------------------------------------- */

	// init reader for ini files
	INIReader reader("assets/settings.ini");

	// load values from ini file
	// first param: section [window], second param: property name, third param: default value
	int width = reader.GetInteger("window", "width", 800);
	int height = reader.GetInteger("window", "height", 800);
	std::string tmp_window_title = reader.Get("window", "title", "ECG");
	const char * window_title = tmp_window_title.c_str();

	/* --------------------------------------------- */
	// Init framework
	/* --------------------------------------------- */


	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		EXIT_WITH_ERROR("Failed to init GLFW")
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		EXIT_WITH_ERROR("Failed to init openGL context or open window")
	}

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		EXIT_WITH_ERROR("Failed to init GLEW")
	}

	glfwSwapInterval(1);

	
	if (!initFramework()) {
		EXIT_WITH_ERROR("Failed to init framework")
	}


	

	/* --------------------------------------------- */
	// Initialize scene and render loop
	/* --------------------------------------------- */

	
	while (!glfwWindowShouldClose(window))
	{	
		glfwPollEvents();
	}




	/* --------------------------------------------- */
	// Destroy framework
	/* --------------------------------------------- */

	destroyFramework();


	/* --------------------------------------------- */
	// Destroy context and exit
	/* --------------------------------------------- */

	glfwDestroyWindow(window);
	glfwTerminate();


	return EXIT_SUCCESS;
}




