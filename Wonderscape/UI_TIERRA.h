#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Model.h"
#include "Camera.h"
#include "shaderClass.h"

// Tamaño de la ventana
const unsigned int width = 1920;
const unsigned int height = 1080;

// Variables globales
bool isPaused = false;
bool messageShown = false;

// Función para manejar entradas del teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if ((key == GLFW_KEY_E) && action == GLFW_PRESS)
    {
        isPaused = !isPaused;

        if (isPaused)
        {
            std::cout << "\n--- Menú: 7 Maravillas del Mundo Moderno ---" << std::endl;
            std::cout << "1. Gran Muralla China" << std::endl;
            std::cout << "2. Petra (Jordania)" << std::endl;
            std::cout << "3. Cristo Redentor (Brasil)" << std::endl;
            std::cout << "4. Machu Picchu (Perú)" << std::endl;
            std::cout << "5. Chichén Itzá (México)" << std::endl;
            std::cout << "6. Coliseo (Italia)" << std::endl;
            std::cout << "7. Taj Mahal (India)" << std::endl;
        }
        else
        {
            std::cout << "Presione E o e para explorar..." << std::endl;
        }
    }
}

void UI_Tierra()
{
    // Inicializar GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Crear ventana
    GLFWwindow* window = glfwCreateWindow(width, height, "WonderScape", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Error al crear la ventana GLFW" << std::endl;
        glfwTerminate();

    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback); // Registrar función de entrada

    // Cargar GLAD
    gladLoadGL();
    glViewport(0, 0, width, height);

    // Cargar shaders
    Shader shaderProgram("default.vert", "default.frag");

    // Configurar iluminación
    glm::vec4 lightColor = glm::vec4(1.0f);
    glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);

    shaderProgram.Activate();
    glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"),
        lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"),
        lightPos.x, lightPos.y, lightPos.z);

    // Activar profundidad
    glEnable(GL_DEPTH_TEST);

    // Cámara
    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 3.0f));

    // Cargar modelo
    Model tierra("models/tierra/scene.gltf");

    float angle = 0.0f;

    std::cout << "Presione E o e para explorar..." << std::endl;

    // Bucle principal
    while (!glfwWindowShouldClose(window))
    {
        if (!isPaused && !messageShown)
        {
            std::cout << "Presione E o e para explorar..." << std::endl;
            messageShown = true;
        }

        if (isPaused)
        {
            messageShown = false;
        }

        // Solo rotar si no está en pausa
        if (!isPaused)
            angle += 0.001f;

        // Matriz de modelo
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));

        // Limpiar pantalla
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Actualizar cámara
        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        // Dibujar modelo con rotación
        tierra.Draw(shaderProgram, camera, model);

        // Mostrar en pantalla
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpiar
    shaderProgram.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
}
