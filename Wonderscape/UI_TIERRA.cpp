#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Config.h"
#include "UI_TIERRA.h"

#include "Model.h"
#include "Camera.h"
#include "shaderClass.h"
#include "Button.h"
#include "AudioManager.h"
#include "TextRenderer.h"

void UI_Tierra(GLFWwindow* window, Camera& camera, Shader& modelShader, Model& earth, TextRenderer& buttonRenderer, AudioManager& audio)
{
    // Boton de Atras
    ButtonClick clickState;
    bool mousePressed = false;
    bool mouseReleased = false;
    Button botonAtras = Button(glm::vec2(20, 1000), glm::vec2(235, 75), "Atras");

    while (true)
    {
        // Tiempo
        float time = glfwGetTime();
        float deltaTime = 0.016f; // Aproximadamente 60 FPS

        // Botones y Mouse
        static bool wasMousePressed = false;
        int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        mousePressed = (state == GLFW_PRESS && !wasMousePressed);
        mouseReleased = (state == GLFW_RELEASE && wasMousePressed);
        wasMousePressed = (state == GLFW_PRESS);

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        mouseY = height - mouseY;

        float normalizedX = (mouseX / width) * 2.0f - 1.0f;
        float normalizedY = (mouseY / height) * 2.0f - 1.0f;
        glm::vec3 mouseWorldPos = glm::vec3(-normalizedX * (float)width / height * 3.0f, -normalizedY * 3.0f, 1.0f);

        // Limpiar pantalla
        glfwPollEvents();
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Boton
        bool hovered = botonAtras.isHovered(mouseX, mouseY);
        bool clicked = false;
        glDisable(GL_DEPTH_TEST);
        botonAtras.render(buttonRenderer, height, hovered, clickState.waiting);
        glEnable(GL_DEPTH_TEST);

        if (hovered) {
            camera.inputsEnabled = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

            if (mousePressed && !clickState.waiting) {
                clicked = true;
                audio.playClickSound();
                clickState.clickTime = glfwGetTime();
                clickState.waiting = true;
            }
        }
        else {
            camera.inputsEnabled = true;
        }

        if (clickState.waiting) {
            double elapsed = glfwGetTime() - clickState.clickTime;

            if (elapsed >= 0.3) {
                clickState.waiting = false;
                break;
            }
        }

        // Actualizar cámara
        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        // Matriz de modelo
        glm::mat4 earthTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 3.7f));
        earthTransform = glm::rotate(earthTransform, glm::radians(time * 20.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        earth.DrawRotation(modelShader, camera, earthTransform);

        glEnable(GL_DEPTH_TEST);
        glfwSwapBuffers(window);
    }
}