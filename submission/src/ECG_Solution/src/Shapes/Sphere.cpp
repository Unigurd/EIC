#include "Sphere.hpp"
#include "glm\matrix.hpp"
#include "glm/ext.hpp"
#include <vector>
#include <iostream>
#include <glm/gtc/constants.hpp>



Sphere::Sphere(unsigned int longSegments, unsigned int latSegments, float radius) {
    // Top and bottom vertex are special cases
    float vs[] = {
        0.0, radius, 0.0,
        0.0, -radius, 0.0
    };

    // initialize vectors. Might be changed to arrays at some point.
    vertices = std::vector<float>(vs, std::end(vs));
    indices = std::vector<unsigned int>((std::vector<unsigned int>::size_type) 0);

    // Populate the vertex vector with vertices
    for (unsigned int j = 1; j < latSegments; j++) {
        float polar = j * glm::pi<float>() / latSegments;
        for (unsigned int i = 0; i < longSegments; i++) {
            float azimuth = i * 2 * glm::pi<float>() / longSegments;

            float x = radius * glm::sin(polar) * glm::cos(azimuth);
            float y = radius * glm::cos(polar);
            float z = radius * glm::sin(polar) * glm::sin(azimuth);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }
    
    // Populate the index vector with the faces that doesn't use the top or bottom vertex.
    for (unsigned int j = 0; j < latSegments - 2; j++) {
        for (unsigned int i = 0; i < longSegments; i++) {
            indices.push_back(i + j * longSegments + 2);
            indices.push_back(((i + 1) % longSegments) + j * longSegments + 2);
            indices.push_back(i + longSegments + j * longSegments + 2);

            indices.push_back(((i + 1) % longSegments) + longSegments + j * longSegments + 2);
            indices.push_back(i + longSegments + j * longSegments + 2);
            indices.push_back(((i + 1) % longSegments) + j * longSegments + 2);
        }
    }

    // Add the top faces to the index vector
    for (unsigned int i = 0; i < longSegments; ++i) {
        indices.push_back(0);
        indices.push_back(((i + 1) % longSegments) + 2);
        indices.push_back(i+2);
    }

    // Add the bottom faces to the index vector
    for (unsigned int i = 0; i < longSegments; ++i) {
        indices.push_back(1);
        indices.push_back(i+2+(longSegments * (latSegments - 2)));
        indices.push_back(((i + 1) % longSegments) + 2 + longSegments * (latSegments - 2));
    }

    initVAO();
}

Sphere::~Sphere()
{
}
