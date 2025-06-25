#ifndef WONDERS_MENU_H
#define WONDERS_MENU_H

#include <GLFW/glfw3.h>

struct GLFWwindow;
class Shader;
class Model;

void WondersMenu(GLFWwindow* window, Shader& modelShader, Model& earth, Model& glassModel, Model& animation, Model& model);

#endif