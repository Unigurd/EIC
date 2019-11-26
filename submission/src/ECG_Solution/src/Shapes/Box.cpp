#include "Box.hpp"
#include <vector>
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

Box::Box(float width, float height, float depth) {
    // The corners of the box
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

    // The sides of the box as triangles
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
    
    // Add corners and triangles to the shape.
    // It is done this way because the vertices and faces are generated dynamically for other shapes
    vertices = std::vector(vs, std::end(vs));
    indices = std::vector(is, std::end(is));

    initVAO();
}

Box::~Box()
{
}
