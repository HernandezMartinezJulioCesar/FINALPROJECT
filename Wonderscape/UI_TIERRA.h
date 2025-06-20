#ifndef UI_TIERRA_H
#define UI_TIERRA_H

#include <GLFW/glfw3.h>

struct GLFWwindow;
class Camera;
class Shader;
class Model;
class TextRenderer;
class AudioManager;

void UI_Tierra(GLFWwindow* window, Camera& camera, Shader& modelShader, Model& earth, TextRenderer& buttonRenderer, AudioManager& audio);

#endif