#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ShaderClass.h"
#include "Config.h"
#include "Model.h"
#include "Camera.h"

#include <chrono>

void PantallaCarga(GLFWwindow* window, Shader& modelShader, Model& lupaModel, float duracion)
{
    Camera camera(width, height, glm::vec3(0.0f));

    double inicio = glfwGetTime();

    glEnable(GL_DEPTH_TEST);

    while (glfwGetTime() - inicio < duracion)
    {
        glfwPollEvents();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Tiempo
        float time = glfwGetTime();
        float deltaTime = 0.016f;

        // Modelo del logo
        float angleDegrees = sin(time * 2.5f) * 45.0f;

        glm::vec3 lupaPosition = glm::vec3(0.0f, 0.0f, 4.0f);
        glm::mat4 lupaTransform = glm::translate(glm::mat4(1.0f), lupaPosition);
        lupaTransform = glm::rotate(lupaTransform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        lupaTransform = glm::rotate(lupaTransform, glm::radians(angleDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
        lupaTransform = glm::scale(lupaTransform, glm::vec3(1.0f));
        lupaModel.DrawRotation(modelShader, camera, lupaTransform);

        modelShader.Activate();
        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        glfwSwapBuffers(window);
    }
}

float rgb(int color) {
    return color / 255.0f;
}

float pos(int point, float base) {
    return (point / base) * 2.0f - 1.0f;
}