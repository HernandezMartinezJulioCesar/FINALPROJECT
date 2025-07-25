#ifndef TEXTURE_SCENE_CLASS_H
#define TEXTURE_SCENE_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>

#include"shaderClass.h"

class Texture_Scene
{
public:
	GLuint ID;
	GLenum type;
	GLuint unit;

	Texture_Scene(const char* image, GLenum texType, GLuint slot, GLenum format, GLenum pixelType);

	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();
};
#endif