#pragma once

#include <vector>


struct Surface {
    float ka;
    float kd;
    float ks;
    int alpha;
};

class Shape
{
protected:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO;
    void initVAO();
    Surface surface;

public:
    void Draw();
    Surface &GetSurface();
};
