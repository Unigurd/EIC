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
#include "Lights.hpp"
#include "readFile.hpp"
namespace fs = std::filesystem;


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
    int width                    = reader.GetInteger("window", "width", 80);
    int height                   = reader.GetInteger("window", "height", 80);
    std::string tmp_window_title = reader.Get("window", "title", "Title not loaded");
    const char * window_title    = tmp_window_title.c_str();
    float fovy                   = (float)reader.GetReal("camera", "fov", 360.0);
    float zNear                  = (float)reader.GetReal("camera", "near", 0.5);
    float zFar                   = (float)reader.GetReal("camera", "far", 50.0);

    // directional light
    float dirLightRed   = (float)reader.GetReal("directionalLight", "red", 50.0);
    float dirLightGreen = (float)reader.GetReal("directionalLight", "green", 50.0);
    float dirLightBlue  = (float)reader.GetReal("directionalLight", "blue", 50.0);
    float dirLightDirX  = (float)reader.GetReal("directionalLight", "dirX", 50.0);
    float dirLightDirY  = (float)reader.GetReal("directionalLight", "dirY", 50.0);
    float dirLightDirZ  = (float)reader.GetReal("directionalLight", "dirZ", 50.0);

    // point light
    float pointLightRed      = (float)reader.GetReal("pointLight", "red", 50.0);
    float pointLightGreen    = (float)reader.GetReal("pointLight", "green", 50.0);
    float pointLightBlue     = (float)reader.GetReal("pointLight", "blue", 50.0);
    float pointLightTransX   = (float)reader.GetReal("pointLight", "transX", 50.0);
    float pointLightTransY   = (float)reader.GetReal("pointLight", "transY", 50.0);
    float pointLightTransZ   = (float)reader.GetReal("pointLight", "transZ", 50.0);
    float pointLightAttConst = (float)reader.GetReal("pointLight", "attenuationConst", 50.0);
    float pointLightAttLin   = (float)reader.GetReal("pointLight", "attenuationLin", 50.0);
    float pointLightAttQuad  = (float)reader.GetReal("pointLight", "attenuationQuad", 50.0);

    // box
    float boxWidth         = (float)reader.GetReal("box", "width", 50.0);
    float boxHeight        = (float)reader.GetReal("box", "height", 50.0);
    float boxDepth         = (float)reader.GetReal("box", "depth", 50.0);
    float boxTransX        = (float)reader.GetReal("box", "transX", 50.0);
    float boxTransY        = (float)reader.GetReal("box", "transY", 50.0);
    float boxTransZ        = (float)reader.GetReal("box", "transZ", 50.0);
    float boxRotX          = (float)reader.GetReal("box", "rotX", 50.0);
    float boxRotY          = (float)reader.GetReal("box", "rotY", 50.0);
    float boxRotZ          = (float)reader.GetReal("box", "rotZ", 50.0);
    float boxScaleX        = (float)reader.GetReal("box", "scaleX", 50.0);
    float boxScaleY        = (float)reader.GetReal("box", "scaleY", 50.0);
    float boxScaleZ        = (float)reader.GetReal("box", "scaleZ", 50.0);
    float boxRed           = (float)reader.GetReal("box", "red", 1.0);
    float boxGreen         = (float)reader.GetReal("box", "green", 1.0);
    float boxBlue          = (float)reader.GetReal("box", "blue", 1.0);
    float boxKA            = (float)reader.GetReal("box", "ka", 10);
    float boxKD            = (float)reader.GetReal("box", "kd", 10);
    float boxKS            = (float)reader.GetReal("box", "ks", 10);
    int boxAlpha           = reader.GetInteger("box", "alpha", 2);
    std::string boxTexture = reader.Get("box", "texture", "");

    // cylinder
    float cylinderHeight        = (float)reader.GetReal("cylinder", "height", 50.0);
    float cylinderRadius        = (float)reader.GetReal("cylinder", "radius", 50.0);
    unsigned int cylinderSides  =     reader.GetInteger("cylinder", "sides", 50);
    float cylinderTransX        = (float)reader.GetReal("cylinder", "transX", 50.0);
    float cylinderTransY        = (float)reader.GetReal("cylinder", "transY", 50.0);
    float cylinderTransZ        = (float)reader.GetReal("cylinder", "transZ", 50.0);
    float cylinderRotX          = (float)reader.GetReal("cylinder", "rotX", 50.0);
    float cylinderRotY          = (float)reader.GetReal("cylinder", "rotY", 50.0);
    float cylinderRotZ          = (float)reader.GetReal("cylinder", "rotZ", 50.0);
    float cylinderScaleX        = (float)reader.GetReal("cylinder", "scaleX", 50.0);
    float cylinderScaleY        = (float)reader.GetReal("cylinder", "scaleY", 50.0);
    float cylinderScaleZ        = (float)reader.GetReal("cylinder", "scaleZ", 50.0);
    float cylinderRed           = (float)reader.GetReal("cylinder", "red", 1.0);
    float cylinderGreen         = (float)reader.GetReal("cylinder", "green", 1.0);
    float cylinderBlue          = (float)reader.GetReal("cylinder", "blue", 1.0);
    float cylinderKA            = (float)reader.GetReal("cylinder", "ka", 10);
    float cylinderKD            = (float)reader.GetReal("cylinder", "kd", 10);
    float cylinderKS            = (float)reader.GetReal("cylinder", "ks", 10);
    int cylinderAlpha           =     reader.GetInteger("cylinder", "alpha", 2);
    std::string cylinderTexture = reader.Get("cylinder", "texture", "");

    // sphere
    unsigned int sphereLongSegments = reader.GetInteger("sphere", "longSegments", 50);
    unsigned int sphereLatSegments  = reader.GetInteger("sphere", "latSegments", 50);
    float sphereRadius              = (float)reader.GetReal("sphere", "radius", 50.0);
    float sphereTransX              = (float)reader.GetReal("sphere", "transX", 50.0);
    float sphereTransY              = (float)reader.GetReal("sphere", "transY", 50.0);
    float sphereTransZ              = (float)reader.GetReal("sphere", "transZ", 50.0);
    float sphereRotX                = (float)reader.GetReal("sphere", "rotX", 50.0);
    float sphereRotY                = (float)reader.GetReal("sphere", "rotY", 50.0);
    float sphereRotZ                = (float)reader.GetReal("sphere", "rotZ", 50.0);
    float sphereScaleX              = (float)reader.GetReal("sphere", "scaleX", 50.0);
    float sphereScaleY              = (float)reader.GetReal("sphere", "scaleY", 50.0);
    float sphereScaleZ              = (float)reader.GetReal("sphere", "scaleZ", 50.0);
    float sphereRed                 = (float)reader.GetReal("sphere", "red", 1.0);
    float sphereGreen               = (float)reader.GetReal("sphere", "green", 1.0);
    float sphereBlue                = (float)reader.GetReal("sphere", "blue", 1.0);
    float sphereKA                  = (float)reader.GetReal("sphere", "ka", 10);
    float sphereKD                  = (float)reader.GetReal("sphere", "kd", 10);
    float sphereKS                  = (float)reader.GetReal("sphere", "ks", 10);
    int sphereAlpha                 = reader.GetInteger("sphere", "alpha", 2);
    std::string sphereTexture       = reader.Get("sphere", "texture", "");


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

        // Set the callbacks
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

    // For reading files
    std::filesystem::path p = "";

    // Read Textures
    std::filesystem::path wood_texture_path  = p / "assets" / "textures" / "wood_texture.dds";
    std::filesystem::path tiles_diffuse_path = p / "assets" / "textures" / "tiles_diffuse.dds";

    // Read shaders
    string vertexShaderGouraudSource   = readFile(p / "assets" / "shaders" / "vertexShaderGouraud.vs");
    string fragmentShaderGouraudSource = readFile(p / "assets" / "shaders" / "fragmentShaderGouraud.fs");
    string vertexShaderPhongSource     = readFile(p / "assets" / "shaders" / "vertexShaderPhong.vs");
    string fragmentShaderPhongSource   = readFile(p / "assets" / "shaders" / "fragmentShaderPhong.fs");

    // Create the point light
    PointLight pointLight;
    pointLight.color       = glm::vec3(pointLightRed, pointLightGreen, pointLightBlue);
    pointLight.position    = glm::vec3(pointLightTransX, pointLightTransY, pointLightTransZ);
    pointLight.attenuation = glm::vec3(pointLightAttQuad, pointLightAttLin, pointLightAttConst);

    // Create the directional light
    DirectionLight dirLight;
    dirLight.color     = glm::vec3(dirLightRed, dirLightGreen, dirLightBlue);
    dirLight.direction = glm::vec3(dirLightDirX, dirLightDirY, dirLightDirZ);

    // Bundle the two lights together to be used as argument to the shaders
    Lights lights;
    lights.dirLight   = dirLight;
    lights.pointLight = pointLight;

    // Specify the properties of the surfaces of the four objects
    Surface boxSurface;
    boxSurface.ka = boxKA;
    boxSurface.kd = boxKD;
    boxSurface.ks = boxKS;
    boxSurface.alpha = boxAlpha;

    Surface cylinderSurface;
    cylinderSurface.ka = cylinderKA;
    cylinderSurface.kd = cylinderKD;
    cylinderSurface.ks = cylinderKS;
    cylinderSurface.alpha = cylinderAlpha;
    
    Surface sphereSurface;
    sphereSurface.ka = sphereKA;
    sphereSurface.kd = sphereKD;
    sphereSurface.ks = sphereKS;
    sphereSurface.alpha = sphereAlpha;

    // Transformations of each object
    Transformation boxTransformation;
    boxTransformation.translation = glm::vec3(boxTransX, boxTransY, boxTransZ);
    boxTransformation.rotation    = glm::vec3(boxRotX, boxRotY, boxRotZ);
    boxTransformation.scaling     = glm::vec3(boxScaleX, boxScaleY, boxScaleZ);

    Transformation cylinderTransformation;
    cylinderTransformation.translation = glm::vec3(cylinderTransX, cylinderTransY, cylinderTransZ);
    cylinderTransformation.rotation    = glm::vec3(cylinderRotX, cylinderRotY, cylinderRotZ);
    cylinderTransformation.scaling     = glm::vec3(cylinderScaleX, cylinderScaleY, cylinderScaleZ);

    Transformation sphereTransformation;
    sphereTransformation.translation = glm::vec3(sphereTransX, sphereTransY, sphereTransZ);
    sphereTransformation.rotation    = glm::vec3(sphereRotX, sphereRotY, sphereRotZ);
    sphereTransformation.scaling     = glm::vec3(sphereScaleX, sphereScaleY, sphereScaleZ);

    glm::vec3 boxColor      = glm::vec3(boxRed, boxGreen, boxBlue);
    glm::vec3 cylinderColor = glm::vec3(cylinderRed, cylinderGreen, cylinderBlue);
    glm::vec3 sphereColor   = glm::vec3(sphereRed, sphereGreen, sphereBlue);


    
    // Generate Shapes
    Box box           = Box(boxWidth, boxHeight, boxDepth, boxSurface, boxTransformation, boxColor, wood_texture_path);
    Cylinder cylinder = Cylinder(cylinderHeight, cylinderRadius, cylinderSides, cylinderSurface, cylinderTransformation, cylinderColor, tiles_diffuse_path);
    Sphere sphere     = Sphere(sphereLongSegments, sphereLatSegments, sphereRadius, sphereSurface, sphereTransformation, sphereColor, tiles_diffuse_path);

    //Box box           = Box(boxWidth, boxHeight, boxDepth, boxSurface, boxTransformation, boxColor, loadDDS(wood_texture_path.string().c_str()));
    //Cylinder cylinder = Cylinder(cylinderHeight, cylinderRadius, cylinderSides, cylinderSurface, cylinderTransformation, cylinderColor, loadDDS(tiles_diffuse_path.string().c_str()));
    //Sphere sphere     = Sphere(sphereLongSegments, sphereLatSegments, sphereRadius, sphereSurface, sphereTransformation, sphereColor, loadDDS(tiles_diffuse_path.string().c_str()));
    
    // Generate shaders
    Shader &boxShader = Shader(
        vertexShaderPhongSource,
        fragmentShaderPhongSource,
        box, lights);

    Shader &cylinderShader = Shader(
        vertexShaderPhongSource,
        fragmentShaderPhongSource,
        cylinder, lights);

    Shader &sphereShader = Shader(
        vertexShaderPhongSource,
        fragmentShaderPhongSource,
        sphere, lights);

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
        BindShader useBox(boxShader, camera);
        box.Draw();
        BindShader useCylinder(cylinderShader, camera);
        cylinder.Draw();
        BindShader useSphere(sphereShader, camera);
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