#include "Box.hpp"
#include <vector>
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

Box::Box(float width, float height, float depth, Surface srfc, Transformation trans, glm::vec3 col) {
    surface = srfc;
    color = col;
    transformation = trans;
    // The corners of the box
    float vs[] = {
         // x-axis normals
         -width / 2,  height / 2, depth / 2,    // top left front
         -1.0, 0.0, 0.0,
         -width / 2,  -height / 2, depth / 2,   // bottom left front
         -1.0, 0.0, 0.0,
         width / 2,  height / 2, depth / 2,     // top right front
         1.0, 0.0, 0.0,
         width / 2,  -height / 2, depth / 2,    // bottom right front
         1.0, 0.0, 0.0,
         width / 2,  height / 2, -depth / 2,    // top right back
         1.0, 0.0, 0.0,
         width / 2,  -height / 2, -depth / 2,   // bottom right back
         1.0, 0.0, 0.0,
         -width / 2,  height / 2, -depth / 2,   // top left back
         -1.0, 0.0, 0.0,
         -width / 2,  -height / 2, -depth / 2,  // bottom left back
         -1.0, 0.0, 0.0,

        // y-axis normals
         -width / 2,  height / 2, depth / 2,    // top left front
         0.0, 1.0, 0.0,
         -width / 2,  -height / 2, depth / 2,   // bottom left front
         0.0, -1.0, 0.0,
         width / 2,  height / 2, depth / 2,     // top right front
         0.0, 1.0, 0.0,
         width / 2,  -height / 2, depth / 2,    // bottom right front
         0.0, -1.0, 0.0,
         width / 2,  height / 2, -depth / 2,    // top right back
         0.0, 1.0, 0.0,
         width / 2,  -height / 2, -depth / 2,   // bottom right back
         0.0, -1.0, 0.0,
         -width / 2,  height / 2, -depth / 2,   // top left back
         0.0, 1.0, 0.0,
         -width / 2,  -height / 2, -depth / 2,  // bottom left back
         0.0, -1.0, 0.0,

         // z-axis normals
         -width / 2,  height / 2, depth / 2,    // top left front
         0.0, 0.0, 1.0,
         -width / 2,  -height / 2, depth / 2,   // bottom left front
         0.0, 0.0, 1.0,
         width / 2,  height / 2, depth / 2,     // top right front
         0.0, 0.0, 1.0,
         width / 2,  -height / 2, depth / 2,    // bottom right front
         0.0, 0.0, 1.0,
         width / 2,  height / 2, -depth / 2,    // top right back
         0.0, 0.0, -1.0,
         width / 2,  -height / 2, -depth / 2,   // bottom right back
         0.0, 0.0, -1.0,
         -width / 2,  height / 2, -depth / 2,   // top left back
         0.0, 0.0, -1.0,
         -width / 2,  -height / 2, -depth / 2,  // bottom left back
         0.0, 0.0, -1.0,

    };

    // The sides of the box as triangles
    unsigned int is[] = {
        16, 17, 18,   // front
        19, 18, 17,
        2, 3, 4,   // right
        5, 4, 3,
        20, 21, 22,   // back
        23, 22, 21,
        6, 7, 0,   // left
        1, 0, 7,
        8, 10, 14,   // top
        12, 14, 10,
        9, 15, 11,   // bottom
        13, 11, 15
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
