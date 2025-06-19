#include "shaderClassF.h"
#include <fstream>
#include <sstream>
#include <iostream>

ShaderF::ShaderF(const char* vertexFile, const char* fragmentFile) {
    std::ifstream vFile(vertexFile);
    std::ifstream fFile(fragmentFile);
    std::stringstream vStream, fStream;

    vStream << vFile.rdbuf();
    fStream << fFile.rdbuf();

    std::string vCode = vStream.str();
    std::string fCode = fStream.str();

    const char* vShaderCode = vCode.c_str();
    const char* fShaderCode = fCode.c_str();

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void ShaderF::Activate() {
    glUseProgram(ID);
}

void ShaderF::Delete() {
    glDeleteProgram(ID);
}
