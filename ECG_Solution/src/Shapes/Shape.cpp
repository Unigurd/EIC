#include "Shape.hpp"
#include <GL\glew.h>
#include <GLFW/glfw3.h>
//#include "../Utils.h"
namespace fs = std::filesystem;

Shape::Shape(fs::path texturePath) : image(loadDDS(texturePath.string().c_str())) { }

void Shape::initVAO() {
    unsigned int VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO); // Check for errors.
    glGenBuffers(1, &EBO);

    // ..:: Initialization code :: ..
    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);
    // 2. copy our vertices array into a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    // 3. copy our index array into an element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    // 4. then set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);  

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    if (image.data) {
        glCompressedTexImage2D(GL_TEXTURE_2D, 0, image.format, image.width, image.height, 0, image.size, image.data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

}

void Shape::Draw() {
    glPointSize(5.0f); // tmp line, remove
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    //glDrawArrays(GL_POINTS, 0, vertices.size());
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // find previous bound and restore it.
}

Surface &Shape::GetSurface() { return surface; }
Transformation &Shape::GetTransformation() { return transformation; }
glm::vec3 Shape::Color() { return color; }
