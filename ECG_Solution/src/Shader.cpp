#include "Shader.hpp"
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "glm\matrix.hpp"
#include "glm/ext.hpp"
#include <string>

Shader::Shader(std::string vertexShaderString, std::string fragmentShaderString, Shape shape, Lights lights) {
    const char *vertexShaderSource   = (const GLchar *)vertexShaderString.c_str();
    const char *fragmentShaderSource = (const GLchar *)fragmentShaderString.c_str();

    // get previously bound shader to restore later
    GLint prevId;
    glGetIntegerv(GL_CURRENT_PROGRAM,&prevId);

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
    shaderID = glCreateProgram();
    glAttachShader(shaderID, vertexShader);
    glAttachShader(shaderID, fragmentShader);
    glLinkProgram(shaderID);
    
    // The shaders have been linked and can now be deleted
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // get the transformations from the shape
    Transformation trans = shape.GetTransformation();

    // Build the model matrix from the arguments
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), trans.translation);
    glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(360.0f * trans.rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(360.0f * trans.rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(360.0f * trans.rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 rotation = rotationZ * rotationY * rotationX;
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), trans.scaling);
	glm::mat4 model = translate * rotation * scale;

    // Separate the two lights
    DirectionLight &dirLight = lights.dirLight;
    PointLight &pointLight   = lights.pointLight;

    glUseProgram(shaderID);

    // Set uniforms related to translating between spaces
    viewProjLocation  = glGetUniformLocation(shaderID, "viewProj");
    cameraPosLocation = glGetUniformLocation(shaderID, "cameraPos");
	int modelLocation = glGetUniformLocation(shaderID, "model");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

    // Set object uniforms
	int colorLocation = glGetUniformLocation(shaderID, "color");
    int kaLocation    = glGetUniformLocation(shaderID, "ka");
    int kdLocation    = glGetUniformLocation(shaderID, "kd");
    int ksLocation    = glGetUniformLocation(shaderID, "ks");
    int alphaLocation = glGetUniformLocation(shaderID, "alpha");
	glUniform3fv(colorLocation, 1, glm::value_ptr(shape.Color()));
    glUniform1f(kaLocation, shape.GetSurface().ka);
    glUniform1f(kdLocation, shape.GetSurface().kd);
    glUniform1f(ksLocation, shape.GetSurface().ks);
    glUniform1i(alphaLocation, shape.GetSurface().alpha);


    // Set the directional light uniforms
	int dirLightColorLocation = glGetUniformLocation(shaderID, "dirLightColor");
	int dirLightDirLocation   = glGetUniformLocation(shaderID, "dirLightDir");
	glUniform3fv(dirLightColorLocation, 1, glm::value_ptr(dirLight.color));
	glUniform3fv(dirLightDirLocation, 1, glm::value_ptr(dirLight.direction));

    // Set the point light uniforms 
	int pointLightColorLocation = glGetUniformLocation(shaderID, "pointLightColor");
	int pointLightPosLocation   = glGetUniformLocation(shaderID, "pointLightPos");
	int attenuationLocation     = glGetUniformLocation(shaderID, "attenuation");
	glUniform3fv(pointLightColorLocation, 1, glm::value_ptr(pointLight.color));
	glUniform3fv(pointLightPosLocation, 1, glm::value_ptr(pointLight.position));
	glUniform3fv(attenuationLocation, 1, glm::value_ptr(pointLight.attenuation));

    // restore previously bound shader
    glUseProgram(prevId);
}

unsigned int Shader::ID() { return shaderID; }
int Shader::ViewProjLocation() { return viewProjLocation; }
int Shader::CameraPosLocation() { return cameraPosLocation; }



BindShader::BindShader(Shader &shader, glm::mat4 viewProjMatrix) {
    // get previously bound shader to restore later
    glGetIntegerv(GL_CURRENT_PROGRAM,&prevId);

    // bind new shader
    glUseProgram(shader.ID());
    glUniformMatrix4fv(shader.ViewProjLocation(), 1, GL_FALSE, glm::value_ptr(viewProjMatrix));
}

BindShader::BindShader(Shader &shader, Camera &camera) {
    // get previously bound shader to restore later
    glGetIntegerv(GL_CURRENT_PROGRAM,&prevId);

    // bind new shader
    glUseProgram(shader.ID());
    glUniformMatrix4fv(shader.ViewProjLocation(), 1, GL_FALSE, glm::value_ptr(camera.ViewProjMatrix()));
    glUniform4fv(shader.CameraPosLocation(), 1, glm::value_ptr(camera.ViewPosMatrix()));
}

// Restore previously used program when binding goes out of scope
BindShader::~BindShader() {
    glUseProgram(prevId);
}

