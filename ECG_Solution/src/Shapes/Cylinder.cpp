#include "Cylinder.hpp"
#include "glm\matrix.hpp"
#include "glm/ext.hpp"
#include <vector>
#include <iostream>


Cylinder::Cylinder(float height, float radius, unsigned int sides) {
    float vs[] = {
        0.0, height/2.0, 0.0,
        0.0, -height/2.0, 0.0
    };


    vertices = std::vector<float>(vs, std::end(vs));
    indices = std::vector<unsigned int>((std::vector<unsigned int>::size_type) sides * 6);

    for (unsigned int i = 1; i <= sides; i++) {
        float radians = (float)glm::radians((float)i * (360.0 / (float)sides));
        float x = glm::sin(radians) * radius;
        float z = glm::cos(radians) * radius;

        // top vertex
        vertices.push_back(x); 
        vertices.push_back(height / 2.0);
        vertices.push_back(z);
        // bottom vertex
        vertices.push_back(x); 
        vertices.push_back(-height / 2.0);
        vertices.push_back(z);
        // top surface
        indices.push_back(0);
        indices.push_back(2*i);
        indices.push_back(((2 * i) % (2* sides)) + 2);
        // bottom surface
        indices.push_back(1);
        indices.push_back((2 * i + 1) % (2 * sides) + 2);
        indices.push_back(2 * i + 1);

        // side triangle 1
        indices.push_back(2 * i);
        indices.push_back((2 * i + 1));
        indices.push_back(((2 * i) % (2* sides)) + 2);

        // side triangle 2
        indices.push_back((2 * i + 1) % (2 * sides) + 2);
        indices.push_back(((2 * i) % (2* sides)) + 2);
        indices.push_back((2 * i + 1));

    }

    initVAO();
}


Cylinder::~Cylinder() { }
