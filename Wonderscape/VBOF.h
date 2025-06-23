#ifndef VBOF_H
#define VBOF_H

#include <glad/glad.h>

class VBOF {
public:
    unsigned int ID;
    VBOF(float* vertices, GLsizeiptr size);

    void Bind();
    void Unbind();
    void Delete();
};

#endif
