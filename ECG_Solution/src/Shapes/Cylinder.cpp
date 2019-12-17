#include "Cylinder.hpp"
#include "glm\matrix.hpp"
#include "glm/ext.hpp"
#include <vector>
#include <iostream>


Cylinder::Cylinder(float height, float radius, unsigned int sides, Surface srfc) {
    surface = srfc;
    // top and bottom middle vertices + normals
    float vs[] = {
        0.0f, height/2.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, -height/2.0f, 0.0f,
        0.0f, -1.0f, 0.0f
    };


    vertices = std::vector<float>(vs, std::end(vs));
    indices = std::vector<unsigned int>((std::vector<unsigned int>::size_type) sides * 6);

    // Add vertices and surfaces to the shape
    for (unsigned int i = 0; i < sides; i++) {
        float radians = (float)glm::radians((float)i * (360.0 / (float)sides));
        float x = glm::sin(radians) * radius;
        float z = glm::cos(radians) * radius;

        // top vertex, top normal
        vertices.push_back(x); 
        vertices.push_back(height / 2.0f);
        vertices.push_back(z);

        vertices.push_back(0); 
        vertices.push_back(0);
        vertices.push_back(1);

        // bottom vertex, bottom normal
        vertices.push_back(x); 
        vertices.push_back(-height / 2.0f);
        vertices.push_back(z);

        vertices.push_back(0.0f); 
        vertices.push_back(0.0f);
        vertices.push_back(-1.0f);

        // top vertex, side normal
        vertices.push_back(x); 
        vertices.push_back(height / 2.0f);
        vertices.push_back(z);

        vertices.push_back(x); 
        vertices.push_back(0.0f);
        vertices.push_back(z);

        // bottom vertex, side normal
        vertices.push_back(x); 
        vertices.push_back(-height / 2.0f);
        vertices.push_back(z);

        vertices.push_back(x); 
        vertices.push_back(0.0f);
        vertices.push_back(z);

        // top surface
        indices.push_back(0);
        indices.push_back(4 * i + 2);
        indices.push_back(((4 * (i + 1)) % (4 * sides)) + 2);

        // bottom surface
        indices.push_back(1);
        indices.push_back((4 * (i+1) + 1) % (4 * sides) + 2);
        indices.push_back((4 * i + 1) % (4 * sides) + 2);

        // side triangle 1
        indices.push_back(4 * i + 4);
        indices.push_back((4 * i + 5));
        indices.push_back(((4 * (i+1) + 2) % (4* sides)) + 2);

        // side triangle 2
        indices.push_back((4 * (i+1) + 3) % (4 * sides) + 2);
        indices.push_back(((4 * (i+1) + 2) % (4* sides)) + 2);
        indices.push_back((4 * i + 5));

    }

    initVAO();
}


Cylinder::~Cylinder() { }
