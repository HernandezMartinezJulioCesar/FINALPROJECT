#pragma once

// Window configuration
constexpr unsigned int width = 1920;
constexpr unsigned int height = 1080;

// Application status
enum AppState {
    MAIN_MENU,
    EXPLORE_SCREEN,
    CREDITS_SCREEN,
    HELP_SCREEN,
    EXIT,
    PREVIEW_SCREEN,
    WONDER_SCREEN,
    TRAVEL_SCREEN
};

// Click management structure
struct ButtonClick {
    int index = -1;
    double clickTime = 0.0;
    bool waiting = false;
};

// Functions
struct GLFWwindow;
class Shader;
class Model;
float rgb(int color);
float pos(int point, float base);
void LoadingScreen(GLFWwindow* window, Shader& modelShader, Model& lupaModel, float duracion = 3.0f);
