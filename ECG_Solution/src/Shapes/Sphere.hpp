#pragma once
#include "Shape.hpp"


class Sphere : public Shape {
public:
    Sphere(unsigned int longSegments, unsigned int latSegments, float radius, Surface srfc);
    ~Sphere();
};

