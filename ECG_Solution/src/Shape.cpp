#include "Shape.hpp"
#include <GL\glew.h>
#include <GLFW/glfw3.h>

void Shape::Draw() {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); // find previous bound and restore it.
}
