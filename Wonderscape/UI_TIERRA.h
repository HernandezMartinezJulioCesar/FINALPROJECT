#ifndef UI_TIERRA_H
#define UI_TIERRA_H

#include <GLFW/glfw3.h>

struct GLFWwindow;
class Shader;
class Model;

void UI_Tierra(GLFWwindow* window, Shader& modelShader, Model& earth, Model& lupaModel, Model& animation);

#endif