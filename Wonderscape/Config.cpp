#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ShaderClass.h"
#include "ShaderClassF.h"
#include "VAOF.h"
#include "VBOF.h"
#include "EBOF.h"
#include "TextureF.h"
#include "Config.h"
#include "Model.h"
#include "Camera.h"

#include <chrono>

float rgb(int color) { return color / 255.0f; }
float pos(int point, float base) { return (point / base) * 2.0f - 1.0f; }

void PantallaCarga(GLFWwindow* window, Shader& modelShader, Model& lupaModel, float duracion)
{
    // Cargar fondo
    ShaderF backgroundShader("background.vert", "background.frag");

    float backgroundVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    unsigned int backgroundIndexes[] = {
        0, 1, 2,
        2, 3, 0
    };

    VAOF vaoF;
    vaoF.Bind();
    VBOF vboF(backgroundVertices, sizeof(backgroundVertices));
    EBOF eboF(backgroundIndexes, sizeof(backgroundIndexes));

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    vaoF.Unbind(); vboF.Unbind(); eboF.Unbind();

    TextureF background("images/backgrounds/loading.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);

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

        // Fondo con textura
        glDisable(GL_DEPTH_TEST);
        backgroundShader.Activate();
        vaoF.Bind();
        background.Bind();
        background.texUnit(backgroundShader.ID, "backgroundTexture", 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glEnable(GL_DEPTH_TEST);

        // Modelo de la lupa
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