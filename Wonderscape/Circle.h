#ifndef CIRCLE_H
#define CIRCLE_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shaderClassF.h"

class Circle {
public:
    Circle(float centerX, float centerY, float radius, int segments = 100);
    void Draw(ShaderF& shader);
    void Delete();

private:
    GLuint VAO, VBO;
    std::vector<float> vertices;
};

#endif
