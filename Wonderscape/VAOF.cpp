#include "VAOF.h"
#include <glad/glad.h>

VAOF::VAOF() {
    glGenVertexArrays(1, &ID);
}

void VAOF::Bind() {
    glBindVertexArray(ID);
}

void VAOF::Unbind() {
    glBindVertexArray(0);
}

void VAOF::Delete() {
    glDeleteVertexArrays(1, &ID);
}
