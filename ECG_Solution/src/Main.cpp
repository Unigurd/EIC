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
#include "glm/ext.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <filesystem>
#include "Camera.hpp"
#include "Cursor.hpp"
#include "Shader.hpp"
#include "WindowInfo.hpp"
#include "Shapes/Box.hpp"
#include "Shapes/Cylinder.hpp"
#include "Shapes/Sphere.hpp"

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

// Handles key presses
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Camera &camera = ((WindowInfo*)glfwGetWindowUserPointer(window))->camera;

    // Close window upon ESC press
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    // Toggle wireframe view upon F1 press
    else if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        camera.toggleWireframe();
    }

    // Toggle backface culling upon F2 press
    else if (key == GLFW_KEY_F2 && action == GLFW_PRESS) {
        camera.toggleBackfaceCulling();
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    Cursor &cursor = ((WindowInfo*)glfwGetWindowUserPointer(window))->cursor;

    // Update the cursor's .pressed according to whether the left mouse button is pressed or not
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        cursor.isPressed = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        cursor.isPressed = false;
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    Cursor &cursor = ((WindowInfo*)glfwGetWindowUserPointer(window))->cursor;
    Camera &camera = ((WindowInfo*)glfwGetWindowUserPointer(window))->camera;

    // Update the position of the cursor
    cursor.MoveTo (xpos, ypos);
    if (cursor.isPressed) {
        // I divide by 1000 to get the rotation speed right. Might not be the right place to do that.
        camera.rotate(glm::vec3(cursor.deltaY / 1000.0, cursor.deltaX / 1000.0 , 0.0));
    }
}

// For some reason scroll_callback is my favorite part of the program
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Camera &camera = ((WindowInfo*)glfwGetWindowUserPointer(window))->camera;
    camera.translate(glm::vec3(0.0, 0.0, yoffset));
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
    float fovy = (float)reader.GetReal("camera", "fov", 360.0);
    float zNear = (float)reader.GetReal("camera", "near", 0.5);
    float zFar = (float)reader.GetReal("camera", "far", 50.0);

    // box
    float boxWidth = (float)reader.GetReal("box", "width", 50.0);
    float boxHeight = (float)reader.GetReal("box", "height", 50.0);
    float boxDepth = (float)reader.GetReal("box", "depth", 50.0);
    float boxTransX = (float)reader.GetReal("box", "transX", 50.0);
    float boxTransY = (float)reader.GetReal("box", "transY", 50.0);
    float boxTransZ = (float)reader.GetReal("box", "transZ", 50.0);
    float boxRotX = (float)reader.GetReal("box", "rotX", 50.0);
    float boxRotY = (float)reader.GetReal("box", "rotY", 50.0);
    float boxRotZ = (float)reader.GetReal("box", "rotZ", 50.0);
    float boxScaleX = (float)reader.GetReal("box", "scaleX", 50.0);
    float boxScaleY = (float)reader.GetReal("box", "scaleY", 50.0);
    float boxScaleZ = (float)reader.GetReal("box", "scaleZ", 50.0);
    float boxRed = (float)reader.GetReal("box", "red", 1.0);
    float boxGreen = (float)reader.GetReal("box", "green", 1.0);
    float boxBlue = (float)reader.GetReal("box", "blue", 1.0);

    // cylinder
    float cylinderHeight = (float)reader.GetReal("cylinder", "height", 50.0);
    float cylinderRadius = (float)reader.GetReal("cylinder", "radius", 50.0);
    unsigned int cylinderSides = reader.GetInteger("cylinder", "sides", 50);
    float cylinderTransX = (float)reader.GetReal("cylinder", "transX", 50.0);
    float cylinderTransY = (float)reader.GetReal("cylinder", "transY", 50.0);
    float cylinderTransZ = (float)reader.GetReal("cylinder", "transZ", 50.0);
    float cylinderRotX = (float)reader.GetReal("cylinder", "rotX", 50.0);
    float cylinderRotY = (float)reader.GetReal("cylinder", "rotY", 50.0);
    float cylinderRotZ = (float)reader.GetReal("cylinder", "rotZ", 50.0);
    float cylinderScaleX = (float)reader.GetReal("cylinder", "scaleX", 50.0);
    float cylinderScaleY = (float)reader.GetReal("cylinder", "scaleY", 50.0);
    float cylinderScaleZ = (float)reader.GetReal("cylinder", "scaleZ", 50.0);
    float cylinderRed = (float)reader.GetReal("cylinder", "red", 1.0);
    float cylinderGreen = (float)reader.GetReal("cylinder", "green", 1.0);
    float cylinderBlue = (float)reader.GetReal("cylinder", "blue", 1.0);

    // sphere
    unsigned int sphereLongSegments = reader.GetInteger("sphere", "longSegments", 50);
    unsigned int sphereLatSegments = reader.GetInteger("sphere", "latSegments", 50);
    float sphereRadius = (float)reader.GetReal("sphere", "radius", 50.0);
    float sphereTransX = (float)reader.GetReal("sphere", "transX", 50.0);
    float sphereTransY = (float)reader.GetReal("sphere", "transY", 50.0);
    float sphereTransZ = (float)reader.GetReal("sphere", "transZ", 50.0);
    float sphereRotX = (float)reader.GetReal("sphere", "rotX", 50.0);
    float sphereRotY = (float)reader.GetReal("sphere", "rotY", 50.0);
    float sphereRotZ = (float)reader.GetReal("sphere", "rotZ", 50.0);
    float sphereScaleX = (float)reader.GetReal("sphere", "scaleX", 50.0);
    float sphereScaleY = (float)reader.GetReal("sphere", "scaleY", 50.0);
    float sphereScaleZ = (float)reader.GetReal("sphere", "scaleZ", 50.0);
    float sphereRed = (float)reader.GetReal("sphere", "red", 1.0);
    float sphereGreen = (float)reader.GetReal("sphere", "green", 1.0);
    float sphereBlue = (float)reader.GetReal("sphere", "blue", 1.0);


    /* --------------------------------------------- */
    // Init framework
    /* --------------------------------------------- */
    GLFWwindow* window;
    {
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

        window = glfwCreateWindow(width, height, window_title, NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            EXIT_WITH_ERROR("Failed to init openGL context or open window")
        }

        // set the callbacks
        glfwSetKeyCallback(window, key_callback);
        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetScrollCallback(window, scroll_callback);
        glfwMakeContextCurrent(window);

        glewExperimental = true;
        GLenum err = glewInit();
        if (GLEW_OK != err)
        {
            EXIT_WITH_ERROR("Failed to init GLEW")
        }

#if _DEBUG
        // Register your debug callback function.
        glDebugMessageCallback(DebugCallback, NULL);
        // Enable synchronous callback. This ensures that your callback function is called
        // right after an error has occurred. 
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

        glfwSwapInterval(1);

        if (!initFramework()) {
            EXIT_WITH_ERROR("Failed to init framework")
        }
    }

    /* --------------------------------------------- */
    // Initialize scene and render loop
    /* --------------------------------------------- */

    // Enable Z depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Read shaders
    std::filesystem::path p = "";
    string vertexShaderSource = readFile(p / "assets" / "shaders" / "vertexShader.txt");
    string fragmentShaderSource = readFile(p / "assets" / "shaders" / "fragmentShader.txt");
    
    // Generate shaders
    Shader &boxShader = Shader(
        vertexShaderSource,
        fragmentShaderSource,
        glm::vec3(boxTransX, boxTransY, boxTransZ),  // translation
        glm::vec3(boxRotX, boxRotY, boxRotZ),  // rotation
        glm::vec3(boxScaleX, boxScaleY, boxScaleZ),  // scale
        glm::vec3(boxRed, boxGreen, boxBlue)); // color

    Shader &cylinderShader = Shader(
        vertexShaderSource,
        fragmentShaderSource,
        glm::vec3(cylinderTransX, cylinderTransY, cylinderTransZ),  // translation
        glm::vec3(cylinderRotX, cylinderRotY, cylinderRotZ),  // rotation
        glm::vec3(cylinderScaleX, cylinderScaleY, cylinderScaleZ),  // scale
        glm::vec3(cylinderRed, cylinderGreen, cylinderBlue)); // color

    Shader &sphereShader = Shader(
        vertexShaderSource,
        fragmentShaderSource,
        glm::vec3(sphereTransX, sphereTransY, sphereTransZ),  // translation
        glm::vec3(sphereRotX, sphereRotY, sphereRotZ),  // rotation
        glm::vec3(sphereScaleX, sphereScaleY, sphereScaleZ),  // scale
        glm::vec3(sphereRed, sphereGreen, sphereBlue)); // color


    // Generate Shapes
    Box box = Box(boxWidth, boxHeight, boxDepth);
    Cylinder cylinder = Cylinder(cylinderHeight, cylinderRadius, cylinderSides);
    Sphere sphere = Sphere(sphereLongSegments, sphereLatSegments, sphereRadius);

    // Create Camera and cursor
    WindowInfo windowInfo = {
        Camera(fovy, height, width, zNear, zFar),
        Cursor()
    };

    // window has a pointer to windowInfo in order to use the camera and cursor in the callbacks
    glfwSetWindowUserPointer(window, (void*)&windowInfo);
    Camera& camera = windowInfo.camera;
    Cursor& cursor = windowInfo.cursor;

	glClearColor(1, 1, 1, 1);
    // Render loop
	while (!glfwWindowShouldClose(window))
	{	
        // Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // poll events
		glfwPollEvents();
        // Bind the shader of each shape and draw it
		BindShader useBox(boxShader, camera.ViewProjMatrix());
        box.Draw();
		BindShader useCylinder(cylinderShader, camera.ViewProjMatrix());
        cylinder.Draw();
		BindShader useSphere(sphereShader, camera.ViewProjMatrix());
        sphere.Draw();

        // swap buffers
		glfwSwapBuffers(window);
	}


	/* --------------------------------------------- */
	// Destroy framework, context and exit
	/* --------------------------------------------- */

	destroyFramework();
	glfwDestroyWindow(window);
	glfwTerminate();

	return EXIT_SUCCESS;
}