#include"VAOmp.h"

// Constructor that generates a VAO ID
VAOmp::VAOmp()
{
	glGenVertexArrays(1, &ID);
}

// Links a VBO Attribute such as a position or color to the VAO
void VAOmp::LinkAttrib(VBOmp& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
	VBO.Bind();
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	VBO.Unbind();
}

// Binds the VAO
void VAOmp::Bind()
{
	glBindVertexArray(ID);
}

// Unbinds the VAO
void VAOmp::Unbind()
{
	glBindVertexArray(0);
}

// Deletes the VAO
void VAOmp::Delete()
{
	glDeleteVertexArrays(1, &ID);
}