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
#include "glm/ext.hpp"
#include <fstream>
#include <iostream>
#include <vector>

string readFile(const string &fileName)
{
    std::ifstream ifs(fileName.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

    std::ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<char> bytes(fileSize);
    ifs.read(bytes.data(), fileSize);

    return string(bytes.data(), fileSize);
}


//The GLSL source for the two kinds of shaders
// Sorry, I didn't have time to put these in files.
// I did this kinda last minute.. ;)
const char* vertexShaderSource =
"#version 430 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 model;\n"
"uniform mat4 viewProj;\n"

"void main()\n"
"{\n"
"    gl_Position = viewProj * model * vec4(aPos,1);\n"
"}\n";

const char* fragmentShaderSource =
"#version 430 core\n"
"uniform vec3 color;\n"
"out vec4 FragColor;\n"
"\n"
"void main()\n"
"{\n"
"    FragColor = vec4(color, 1.0f);\n"
"}\n";

/* --------------------------------------------- */
// Classes
/* --------------------------------------------- */

// The Camera class
// Will be moved into its own file
class Camera {
private:
    glm::mat4 projection; // This is constant
    glm::mat4 translation;
    glm::mat4 rotation;
    float rotationX, rotationY; // 1.0 is one full rotation
    glm::mat4 viewProj;
    void updateViewProj();

public:
    Camera(float fov, float height, float width, float zNear, float zFar);
    Camera(float fov, float height, float width, float zNear, float zFar, glm::vec3 trans, glm::vec3 rot);
    glm::mat4 ViewProjMatrix();
    void translate(glm::vec3 trans);
    void rotate(glm::vec3 rot);
};

Camera::Camera(float fov, float height, float width, float zNear, float zFar) {
    float aspect = width / height;
    projection = glm::perspective(fov, aspect, zNear, zFar); // Not changed again since it is constant.
    translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 6.0f));
    rotationX = 1.0f; rotationY = 1.0f;
    updateViewProj();
}

Camera::Camera(float fov, float height, float width, float zNear, float zFar, glm::vec3 trans, glm::vec3 rot) {
    float aspect = width / height;
    projection = glm::perspective(fov, aspect, zNear, zFar); // Not changed again since it is constant.
    translation = glm::translate(glm::mat4(1.0f), trans);
    rotationX = 1.0f; rotationY = 1.0f;
    updateViewProj();
}

void Camera::updateViewProj(){
    glm::mat4 rotMatX = glm::rotate(glm::mat4(1.0f), glm::radians(360 * rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotMatY = glm::rotate(glm::mat4(1.0f), glm::radians(360 * rotationY), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 view = glm::inverse(rotMatY * rotMatX * translation);
    viewProj = projection * view;
}

void Camera::translate(glm::vec3 trans) {
    translation = glm::translate(translation, trans);
    updateViewProj();
}

void Camera::rotate(glm::vec3 rot) {
    rotationX += rot[0]; rotationY += rot[1];
    // Make sure vertical rotation doesn't exceed +- 90 degrees
    float gap = 0.01f;
    if (rotationX >= 1.25f) {
        rotationX = 1.25f - gap;
    }

    if (rotationX <= 0.75f) {
        rotationX = 0.75f + gap;
    }

    updateViewProj();
 }

// Returns the ViewProjMatrix for use in the main loop
glm::mat4 Camera::ViewProjMatrix(){
    return viewProj;
}


// Cursor class to keep track of when the mouse is pressed, and how much it has moved since last checked
// Will also be put into own file
class Cursor {
private:
    double xpos, ypos;
public:
    Cursor();
    bool isPressed;
    double deltaX, deltaY;
    void MoveTo(double x, double y);
};

Cursor::Cursor() { isPressed = false; }

void Cursor::MoveTo(double x, double y) {
    deltaX = xpos - x;
    deltaY = ypos - y;
    xpos = x; ypos = y;
}

// To be set as WindowUserPointer or whatever it's called
// so camera and cursor can be passed to the callbacks
struct WindowInfo {
    Camera camera;
    Cursor cursor;
};

// Will be refactored into a shader class when time permits
unsigned int buildShader(glm::vec3 pos, glm::vec3 rot, glm::vec3 sca, glm::vec3 col) {
    // Create the vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Create the fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    // Build the shader program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // The shaders have been linked and can now be deleted
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Build the model matrix from the arguments
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), pos);
    glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(360.0f * rot[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(360.0f * rot[1]), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(360.0f * rot[2]), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 rotation = rotationZ * rotationY * rotationX;
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), sca);
	glm::mat4 model = translate * rotation * scale;

    // Set the model and color uniforms in the shader program
    glUseProgram(shaderProgram);
	int modelLocation = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
	int colorLocation = glGetUniformLocation(shaderProgram, "color");
	glUniform3fv(colorLocation, 1, glm::value_ptr(col));

	return shaderProgram;
}

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


    // Build the shaders for the two different teapots
    unsigned int redShader = buildShader(glm::vec3(1.5f, 1.0f, 0.0f),  // translation
        glm::vec3(0.0f, 0.0f, 0.0f),  // rotation
        glm::vec3(1.0f, 2.0f, 1.0f),  // scale
        glm::vec3(1.0f, 0.0f, 0.0f)); // color

    unsigned int blueShader = buildShader(glm::vec3(-1.5f, -1.0f, 0.0f),
        glm::vec3(0.0f, 0.5f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f));


    // location of the view-projection matrices in the two shaders
    int redProjLocation = glGetUniformLocation(redShader, "viewProj");
    int blueProjLocation = glGetUniformLocation(blueShader, "viewProj");
    


    WindowInfo windowInfo = {
        Camera(fovy, height, width, zNear, zFar),
        Cursor()
    };
    glfwSetWindowUserPointer(window, (void*)&windowInfo);

    Camera& camera = windowInfo.camera;
    Cursor& cursor = windowInfo.cursor;

	
    glm::vec3 trans = glm::vec3(0.0f, 0.0f, 6.0f);
    glm::vec3 rot = glm::vec3(1.0f, 1.0f, 1.0f);

	glClearColor(1, 1, 1, 1);
	while (!glfwWindowShouldClose(window))
	{	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwPollEvents();
		glUseProgram(redShader);
        glUniformMatrix4fv(redProjLocation, 1, GL_FALSE, glm::value_ptr(camera.ViewProjMatrix()));
		drawTeapot();
		glUseProgram(blueShader);
	    glUniformMatrix4fv(blueProjLocation, 1, GL_FALSE, glm::value_ptr(camera.ViewProjMatrix()));
		drawTeapot();
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