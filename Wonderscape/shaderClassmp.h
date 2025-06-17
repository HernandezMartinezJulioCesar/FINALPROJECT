#ifndef SHADER_CLASSMP_H
#define SHADER_CLASSMP_H

#include<glad/glad.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

std::string get_file_content(const char* filename);

class Shadermp {

public:
	GLuint ID;
	Shadermp(const char* vertexFile, const char* fragmentFile);

	void Activate();
	void Delete();
};



#endif