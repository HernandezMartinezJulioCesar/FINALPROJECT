#include "VBOF.h"
#include <glad/glad.h>

VBOF::VBOF(float* vertices, GLsizeiptr size) {
    glGenBuffers(1, &ID);
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void VBOF::Bind() {
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBOF::Unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBOF::Delete() {
    glDeleteBuffers(1, &ID);
}
