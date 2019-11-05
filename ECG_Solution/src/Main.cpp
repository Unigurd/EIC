/*
* Copyright 2019 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/

#include <string>
#include <sstream>
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "Utils.h"
#include "glm\matrix.hpp"
#include <glm/gtc/type_ptr.hpp>

float autoRot() {
	return ((sin(glfwGetTime()) / 2.0f) + 0.5f);
}

/* --------------------------------------------- */
// Prototypes
/* --------------------------------------------- */
// CHANGE VERSION
const char* vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 model;\n"
"uniform mat4 viewProj;\n"

"void main()\n"
"{\n"
"    gl_Position = viewProj * model * vec4(aPos,1);\n"
"}\n";

const char* fragmentShaderSource =
"#version 330 core\n"
"uniform vec3 color;\n"
"out vec4 FragColor;\n"
"\n"
"void main()\n"
"{\n"
"    FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\n";

unsigned int buildShader(glm::vec3 pos, glm::vec3 rot, glm::vec3 sca, glm::vec3 col) {
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glm::mat4 translate = glm::translate(glm::mat4(1.0f), pos);
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), rot);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), sca);
	glm::mat4 model = translate * rotate * scale;

    glUseProgram(shaderProgram);
	int modelLocation = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
	int colorLocation = glGetUniformLocation(shaderProgram, "color");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(col));

	return shaderProgram;
}

/* --------------------------------------------- */
// Global variables
/* --------------------------------------------- */


/* --------------------------------------------- */
// Callbacks
/* --------------------------------------------- */

static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg) {
    std::stringstream stringStream;
    std::string sourceString;
    std::string typeString;
    std::string severityString; 
    
    switch (source) {
        case GL_DEBUG_SOURCE_API: {
            sourceString = "API";
            break;
        }
        case GL_DEBUG_SOURCE_APPLICATION: {
            sourceString = "Application";
            break;
        }
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
            sourceString = "Window System";
            break;
        }
        case GL_DEBUG_SOURCE_SHADER_COMPILER: {
            sourceString = "Shader Compiler";
            break;
        }
        case GL_DEBUG_SOURCE_THIRD_PARTY: {
            sourceString = "Third Party";
            break;
        }
        case GL_DEBUG_SOURCE_OTHER: {
            sourceString = "Other";
            break;
        }
        default: {
            sourceString = "Unknown";
            break;
        }
    }
 
    switch (type) {
        case GL_DEBUG_TYPE_ERROR: {
            typeString = "Error";
            break;
        }
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
            typeString = "Deprecated Behavior";
            break;
        }
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
            typeString = "Undefined Behavior";
            break;
        }
        case GL_DEBUG_TYPE_PORTABILITY_ARB: {
            typeString = "Portability";
            break;
        }
        case GL_DEBUG_TYPE_PERFORMANCE: {
            typeString = "Performance";
            break;
        }
        case GL_DEBUG_TYPE_OTHER: {
            typeString = "Other";
            break;
        }
        default: {
            typeString = "Unknown";
            break;
        }
    }
 
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH: {
            severityString = "High";
            break;
        }
        case GL_DEBUG_SEVERITY_MEDIUM: {
            severityString = "Medium";
            break;
        }
        case GL_DEBUG_SEVERITY_LOW: {
            severityString = "Low";
            break;
        }
        default: {
            severityString = "Unknown";
            break;
        }
    }
 
    stringStream << "OpenGL Error: " << msg;
    stringStream << " [Source = " << sourceString;
    stringStream << ", Type = " << typeString;
    stringStream << ", Severity = " << severityString;
    stringStream << ", ID = " << id << "]";
 
    return stringStream.str();
}



void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                   GLsizei length, const GLchar* message, const GLvoid* userParam) 
{
    std::string error = FormatDebugOutput(source, type, id, severity, message);
    std::cout << error << std::endl;
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
	int width = reader.GetInteger("window", "width", 80);
	int height = reader.GetInteger("window", "height", 80);
	std::string tmp_window_title = reader.Get("window", "title", "Title not loaded");
	const char * window_title = tmp_window_title.c_str();
	double fovy = reader.GetReal("camera", "fov", 360.0);
	double zNear = reader.GetReal("camera", "near", 0.5);
	double zFar = reader.GetReal("camera", "far", 50.0);

	/* --------------------------------------------- */
	// Init framework
	/* --------------------------------------------- */


	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		EXIT_WITH_ERROR("Failed to init GLFW")
	}

    #if _DEBUG
        // Create a debug OpenGL context or tell your OpenGL library (GLFW, SDL) to do so.
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    #endif 

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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

    #if _DEBUG
    	// Register your callback function.
    	glDebugMessageCallback(DebugCallback, NULL);
    	// Enable synchronous callback. This ensures that your callback function is called
    	// right after an error has occurred. 
    	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    #endif
  

	glfwSwapInterval(1);

	
	if (!initFramework()) {
		EXIT_WITH_ERROR("Failed to init framework")
	}


	/* --------------------------------------------- */
	// Initialize scene and render loop
	/* --------------------------------------------- */

    /* Enable Z depth testing so objects closest to the viewpoint are in front of objects further away */
	// Taken from  khronos.org
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); 
    
	
	unsigned int redShader = buildShader(glm::vec3(1.5f, 1.0f, 0.0f), 
		                                 glm::vec3(0.0f, 0.0f, 0.0f), 
	                                     glm::vec3(1.0f, 2.0f, 1.0f), 
	                                     glm::vec3(1.0f, 0.0f, 0.0f));

	unsigned int blueShader = buildShader(glm::vec3(-1.5f, -1.0f, 0.0f), 
		                                 glm::vec3(0.0f, 0.5f, 0.0f), 
	                                     glm::vec3(1.0f, 1.0f, 1.0f), 
	                                     glm::vec3(0.0f, 0.0f, 1.0f));


	double aspect = (double)width / (double)height;
	glm::mat4 projection = glm::perspective(45.0, aspect, zNear, zFar);
	int redProjLocation = glGetUniformLocation(redShader, "viewProj");
	int blueProjLocation = glGetUniformLocation(blueShader, "viewProj");

	glClearColor(1, 1, 1, 1);
	
	while (!glfwWindowShouldClose(window))
	{	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwPollEvents();
	    glm::mat4 distance = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f, 0.0f, 6.0f));
	    glm::mat4 rotateCam = glm::rotate(glm::mat4(1.0f), glm::radians(360.0f*autoRot()), glm::vec3(0.0f, 1.0f, 0.0f));
	    glm::mat4 view = glm::inverse(rotateCam * distance);
	    glm::mat4 viewProj = projection * view;
		glUseProgram(redShader);
	    glUniformMatrix4fv(redProjLocation, 1, GL_FALSE, glm::value_ptr(viewProj));
		drawTeapot();
		glUseProgram(blueShader);
	    glUniformMatrix4fv(blueProjLocation, 1, GL_FALSE, glm::value_ptr(viewProj));
		drawTeapot();
		glfwSwapBuffers(window);
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