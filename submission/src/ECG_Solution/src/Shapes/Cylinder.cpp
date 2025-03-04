#include "Cylinder.hpp"
#include "glm\matrix.hpp"
#include "glm/ext.hpp"
#include <vector>
#include <iostream>
#include <cmath>


Cylinder::Cylinder(float height, float radius, unsigned int sides, Surface srfc, Transformation trans, glm::vec3 col, fs::path texturePath) : Shape::Shape(texturePath) {
    surface = srfc;
    color = col;
    transformation = trans;
    // top and bottom middle vertices + normals
    float vs[] = 
    {
        0.0f, height/2.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.5f, 0.5f,
        0.0f, -height/2.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.5f, 0.5f
    };


    vertices = std::vector<float>(vs, std::end(vs));
    indices = std::vector<unsigned int>((std::vector<unsigned int>::size_type) sides * 6);

    // Add vertices and surfaces to the shape
    for (unsigned int i = 0; i < sides; i++) {
        float radians = (float)glm::radians((float)i * (360.0 / (float)sides));
        float xUnit = glm::sin(radians);
        float zUnit = glm::cos(radians);

        float x = xUnit * radius;
        float z = zUnit * radius;

        // top vertex, top normal
        vertices.push_back(x); 
        vertices.push_back(height / 2.0f);
        vertices.push_back(z);

        vertices.push_back(0.0f); 
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);

        vertices.push_back((xUnit + 1.0f) / 2.0f);
        vertices.push_back((zUnit + 1.0f) / 2.0f);

        // bottom vertex, bottom normal
        vertices.push_back(x); 
        vertices.push_back(-height / 2.0f);
        vertices.push_back(z);

        vertices.push_back(0.0f); 
        vertices.push_back(-1.0f);
        vertices.push_back(0.0f);

        vertices.push_back((xUnit + 1.0f) / 2.0f);
        vertices.push_back((zUnit + 1.0f) / 2.0f);

        // top vertex, side normal
        vertices.push_back(x); 
        vertices.push_back(height / 2.0f);
        vertices.push_back(z);

        vertices.push_back(x); 
        vertices.push_back(0.0f);
        vertices.push_back(z);

        vertices.push_back(std::fmodf ((((float)i / (float)sides) + 0.5f), 1.0f));
        //vertices.push_back(-(float)i / (float)sides);
        vertices.push_back(1.0f);
        
        // bottom vertex, side normal
        vertices.push_back(x); 
        vertices.push_back(-height / 2.0f);
        vertices.push_back(z);

        vertices.push_back(x); 
        vertices.push_back(0.0f);
        vertices.push_back(z);

        vertices.push_back(std::fmodf ((((float)i / (float)sides) + 0.5f), 1.0f));
        vertices.push_back(0.0f);


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
