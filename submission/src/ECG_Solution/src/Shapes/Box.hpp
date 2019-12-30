#pragma once

#include "Shape.hpp"
#include <string>
#include <filesystem>
namespace fs = std::filesystem;

class Box : public Shape {
public:
    Box(float width, float height, float depth, Surface srfc, Transformation trans, glm::vec3 col, fs::path texturePath);
    //Box(float width, float height, float depth, Surface srfc, Transformation trans, glm::vec3 col, DDSImage &&img);
    ~Box();
};

