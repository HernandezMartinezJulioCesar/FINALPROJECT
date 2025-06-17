#ifndef TEXTUREF_H
#define TEXTUREF_H

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <string>
#include <iostream>

class TextureF {
public:
    GLuint ID;
    GLenum type;

    TextureF(const char* imagePath, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);
    void texUnit(GLuint shaderID, const char* uniform, GLuint unit);
    void Bind();
    void Unbind();
    void Delete();
};

#endif
