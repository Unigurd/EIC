#include "Shader.hpp"
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "glm\matrix.hpp"
#include "glm/ext.hpp"
#include <string>

// Will take the source as argument when I get around to reading them from file
Shader::Shader(std::string vertexShaderString, std::string fragmentShaderString, glm::vec3 pos, glm::vec3 rot, glm::vec3 sca, glm::vec3 col, Shape shape) {
    const char *vertexShaderSource = (const GLchar *)vertexShaderString.c_str();
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

    // Build the model matrix from the arguments
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), pos);
    glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(360.0f * rot[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(360.0f * rot[1]), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(360.0f * rot[2]), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 rotation = rotationZ * rotationY * rotationX;
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), sca);
	glm::mat4 model = translate * rotation * scale;

    // tmp stuff
    glm::vec3 lightColor = glm::vec3(1.0, 1.0, 1.0);
    glm::vec3 lightDirection = glm::vec3(0.0, -1.0, -1.0);
    glm::vec3 lightPos = glm::vec3(0.0, 0.0, 0.0);
    float attConst = 1.0;
    float attLin = 0.4;
    float attQuad = 0.1;

    // Set the model and color uniforms in the shader program
    glUseProgram(shaderID);
	int modelLocation = glGetUniformLocation(shaderID, "model");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
	int colorLocation = glGetUniformLocation(shaderID, "color");
	glUniform3fv(colorLocation, 1, glm::value_ptr(col));

	int dirLightColorLocation = glGetUniformLocation(shaderID, "dirLightColor");
	glUniform3fv(dirLightColorLocation, 1, glm::value_ptr(lightColor));
	int dirLightDirLocation = glGetUniformLocation(shaderID, "dirLightDir");
	glUniform3fv(dirLightDirLocation, 1, glm::value_ptr(lightDirection));

	int pointLightColorLocation = glGetUniformLocation(shaderID, "pointLightColor");
	glUniform3fv(pointLightColorLocation, 1, glm::value_ptr(lightColor));
	int pointLightPosLocation = glGetUniformLocation(shaderID, "pointLightPos");
	glUniform3fv(pointLightPosLocation, 1, glm::value_ptr(lightPos));

	int attConstLocation = glGetUniformLocation(shaderID, "attConst");
	glUniform1f(attConstLocation, attConst);
	int attLinLocation = glGetUniformLocation(shaderID, "attLin");
	glUniform1f(attLinLocation, attLin);
	int attQuadLocation = glGetUniformLocation(shaderID, "attQuad");
	glUniform1f(attQuadLocation, attQuad);

    viewProjLocation = glGetUniformLocation(shaderID, "viewProj");
    cameraPosLocation  = glGetUniformLocation(shaderID, "cameraPos");

    int kaLocation    = glGetUniformLocation(shaderID, "ka");
    int kdLocation    = glGetUniformLocation(shaderID, "kd");
    int ksLocation    = glGetUniformLocation(shaderID, "ks");
    int alphaLocation = glGetUniformLocation(shaderID, "alpha");
    glUniform1f(kaLocation, shape.GetSurface().ka);
    glUniform1f(kdLocation, shape.GetSurface().kd);
    glUniform1f(ksLocation, shape.GetSurface().ks);
    glUniform1i(alphaLocation, shape.GetSurface().alpha);


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

