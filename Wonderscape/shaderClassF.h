#ifndef SHADERCLASSF_H
#define SHADERCLASSF_H

#include <string>
#include <glad/glad.h>

class ShaderF {
public:
    unsigned int ID;
    ShaderF(const char* vertexFile, const char* fragmentFile);

    void Activate();
    void Delete();
};

#endif
