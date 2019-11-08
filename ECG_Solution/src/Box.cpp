#include "Box.hpp"
#include <vector>
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

Box::Box(float width, float height, float depth) {
    float vs[] = {
         -width / 2,  height / 2, depth / 2,    // top left front
         -width / 2,  -height / 2, depth / 2,   // bottom left front
         width / 2,  height / 2, depth / 2,     // top right front
         width / 2,  -height / 2, depth / 2,    // bottom right front
         width / 2,  height / 2, -depth / 2,    // top right back
         width / 2,  -height / 2, -depth / 2,   // bottom right back
         -width / 2,  height / 2, -depth / 2,   // top left back
         -width / 2,  -height / 2, -depth / 2,  // bottom left back
    };

    unsigned int is[] = {
        0, 1, 2,   // front
        3, 2, 1,
        2, 3, 4,   // right
        5, 4, 3,
        4, 5, 6,   // back
        7, 6, 5,
        6, 7, 0,   // left
        1, 0, 7,
        0, 2, 6,   // top
        4, 6, 2,
        1, 7, 3,
        5, 3, 7
    };

    vertices = std::vector(vs, std::end(vs));
    indices = std::vector(is, std::end(is));

    unsigned int VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO); // check for errors
    glGenBuffers(1, &EBO);

    // ..:: Initialization code :: ..
    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);
    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    // 3. copy our index array in a element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    // 4. then set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

Box::~Box()
{
}

void Box::Draw() {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); // find previous bound and restore it.
}
