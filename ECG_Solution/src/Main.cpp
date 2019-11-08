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
#include "Box.hpp"

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
    Camera &camera = ((WindowInfo*)glfwGetWindowUserPointer(window))->camera;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        camera.toggleWireframe();
    }
    else if (key == GLFW_KEY_F2 && action == GLFW_PRESS) {
        camera.toggleBackfaceCulling();
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    Cursor &cursor = ((WindowInfo*)glfwGetWindowUserPointer(window))->cursor;
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
    cursor.MoveTo (xpos, ypos);
    if (cursor.isPressed) {
        // I think I just divide by 1000 to get the rotation speed right. Might not be the right place to do that.
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
    double fovy = reader.GetReal("camera", "fov", 360.0);
    double zNear = reader.GetReal("camera", "near", 0.5);
    double zFar = reader.GetReal("camera", "far", 50.0);


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

    std::filesystem::path p = "";

     string vertexShaderSource = readFile(p / "assets" / "shaders" / "vertexShader.txt");
     string fragmentShaderSource = readFile(p / "assets" / "shaders" / "fragmentShader.txt");
    

    // float sheight = 2.0f;
    // float swidth = 1.0f;
    // float sdepth = 0.5f;

    // float vs[] = {
    //      -swidth/2,  sheight/2, sdepth/2,    // top left front
    //      -swidth/2,  -sheight/2, sdepth/2,   // bottom left front
    //      swidth/2,  sheight/2, sdepth/2,     // top right front
    //      swidth/2,  -sheight/2, sdepth/2,    // bottom right front
    //      swidth/2,  sheight/2, -sdepth/2,    // top right back
    //      swidth/2,  -sheight/2, -sdepth/2,   // bottom right back
    //      -swidth/2,  sheight/2, -sdepth/2,   // top left back
    //      -swidth/2,  -sheight/2, -sdepth/2,  // bottom left back
    // };

    // unsigned int is[] = {
    //     0, 1, 2,   // front
    //     3, 2, 1,
    //     2, 3, 4,   // right
    //     5, 4, 3,
    //     4, 5, 6,   // back
    //     7, 6, 5,
    //     6, 7, 0,   // left
    //     1, 0, 7,
    //     0, 2, 6,   // top
    //     4, 6, 2,
    //     1, 7, 3,
    //     5, 3, 7
    // };  

    // std::vector vertices = std::vector(vs, std::end(vs));
    // std::vector indices = std::vector(is, std::end(is));

    // // for (int a = 0; a < 36; a++) {
    // //         std::cout << ((unsigned int *)static_cast<void*>(indices.data()))[a] << std::endl;
    // // }

    // std::cout << vertices.size() << "    " << indices.size() << std::endl;

    // unsigned int VAO, VBO, EBO;
    // glGenVertexArrays(1, &VAO);  
    // glGenBuffers(1, &VBO); // check for errors
    // glGenBuffers(1, &EBO);

    // // ..:: Initialization code :: ..
    // // 1. bind Vertex Array Object
    // glBindVertexArray(VAO);
    // // 2. copy our vertices array in a vertex buffer for OpenGL to use
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    // // 3. copy our index array in a element buffer for OpenGL to use
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    // // 4. then set the vertex attributes pointers
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);  

    Shader &redShader = Shader(
        vertexShaderSource,
        fragmentShaderSource,
        glm::vec3(1.5f, 1.0f, 0.0f),  // translation
        glm::vec3(0.0f, 0.0f, 0.0f),  // rotation
        glm::vec3(1.0f, 1.0f, 1.0f),  // scale
        glm::vec3(1.0f, 0.0f, 0.0f)); // color


    // location of the view-projection matrices in the two shaders
    int redProjLocation = glGetUniformLocation(redShader.ID(), "viewProj");

    WindowInfo windowInfo = {
        Camera(fovy, height, width, zNear, zFar),
        Cursor()
    };
    glfwSetWindowUserPointer(window, (void*)&windowInfo);

    Camera& camera = windowInfo.camera;
    Cursor& cursor = windowInfo.cursor;
    Box box = Box(2.0f, 1.0f, 1.5f);
	
    glm::vec3 trans = glm::vec3(0.0f, 0.0f, 6.0f);
    glm::vec3 rot = glm::vec3(1.0f, 1.0f, 1.0f);

	glClearColor(1, 1, 1, 1);
	while (!glfwWindowShouldClose(window))
	{	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwPollEvents();
		BindShader useRed(redShader);
        glUniformMatrix4fv(redProjLocation, 1, GL_FALSE, glm::value_ptr(camera.ViewProjMatrix()));
        //glBindVertexArray(VAO);
        //glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        //glBindVertexArray(0); // find previous bound and restore it.
        box.Draw();

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