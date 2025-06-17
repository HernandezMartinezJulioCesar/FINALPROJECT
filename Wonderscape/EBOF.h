#ifndef EBOF_H
#define EBOF_H

#include <glad/glad.h>

class EBOF {
public:
    unsigned int ID;
    EBOF(unsigned int* indices, GLsizeiptr size);

    void Bind();
    void Unbind();
    void Delete();
};

#endif
