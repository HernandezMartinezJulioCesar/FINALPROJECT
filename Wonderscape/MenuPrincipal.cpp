#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>

#include "Config.h"
#include "MenuPrincipal.h"
#include "UI_TIERRA.h"

#include "VAOF.h"
#include "VBOF.h"
#include "EBOF.h"
#include "Model.h"
#include "Camera.h"
#include "shaderClass.h"
#include "shaderClassF.h"
#include "TextRenderer.h"
#include "Circle.h"
#include "Button.h"
#include "Texture.h"
#include "TextureF.h"
#include "AudioManager.h"

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <Windows.h>

float rgb(int color) {
    return color / 255.0f;
}

int MenuPrincipal() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Wonder Scapes", monitor, NULL);

    // GLFWwindow* window = glfwCreateWindow(width, height, "Wonder Scapes", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, width, height);

    // Cargar �cono de la ventana
    int iconWidth, iconHeight, channels;
    unsigned char* iconPixels = stbi_load("icono.png", &iconWidth, &iconHeight, &channels, 4);
    if (iconPixels) {
        GLFWimage icon;
        icon.width = iconWidth;
        icon.height = iconHeight;
        icon.pixels = iconPixels;
        glfwSetWindowIcon(window, 1, &icon);
        stbi_image_free(iconPixels);
    }
    else {
        std::cerr << "No se pudo cargar el icono de la ventana." << std::endl;
    }

    // Cargar audio
    AudioManager audio;
    audio.playBackgroundMusic("media/environment.mp3");
    audio.setMusicVolume(1.0f);
    audio.setEffectsVolume(0.4f);

    // Cargar texto
    TextRenderer textRenderer(width, height), buttonRenderer(width, height), creditsRenderer(width, height);
    textRenderer.Load("fonts/River Adventurer.ttf", 200);
    buttonRenderer.Load("fonts/JungleAdventurer.ttf", 100);
    creditsRenderer.Load("fonts/Cute Dino.ttf", 80);

    // Cargar fondo
    ShaderF backgroundShader("background.vert", "background.frag");

    float backgroundVertices[] = {
        // pos        // tex
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    unsigned int backgroundIndices[] = {
        0, 1, 2,
        2, 3, 0
    };

    VAOF vaoF;
    vaoF.Bind();
    VBOF vboF(backgroundVertices, sizeof(backgroundVertices));
    EBOF eboF(backgroundIndices, sizeof(backgroundIndices));

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    vaoF.Unbind(); vboF.Unbind(); eboF.Unbind();

    TextureF backgroundMenu("fondoMenu.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
    TextureF backgroundCH("Creditshelp.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);

    // C�rculo base para la Tierra
    ShaderF circleShader("circle.vert", "circle.frag");
    Circle circle(1.38f, 0.0f, 0.80f);

    // Cargar modelos de la Tierra y las Nubes
    Shader modelShader("default.vert", "defaultModel.frag");
    glm::vec4 lightColor = glm::vec4(1.0f);

    modelShader.Activate();
    glUniform4f(glGetUniformLocation(modelShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);

    Camera camera(width, height, glm::vec3(0.0f));
    Model earth("models/tierra/scene.gltf");
    Model clouds("models/nubes/scene.gltf");
    Model uni("models/ModelUni/UNI.gltf");
    Model lupa("models/MagnifyingGlass/scene.gltf");
    Model animation("models/LoadingScreen/scene.gltf");

    Texture Textura("models/ModelUni/textures/Image_0.jpg", "albedo", 0);
    for (auto& mesh : uni.meshes) {
        mesh.textures.clear();
        mesh.textures.push_back(Textura);
    }
    Textura = Texture("models/MagnifyingGlass/texture/Image_0.jpg", "albedo", 0);
    for (auto& mesh : lupa.meshes) {
        mesh.textures.clear();
        mesh.textures.push_back(Textura);
    }

    // Botones del Menu Principal
    std::vector<Button> botonesMenu = {
        Button(glm::vec2(170, 500), glm::vec2(370, 74), "Explorar"),
        Button(glm::vec2(170, 400), glm::vec2(240, 74), "Ayuda"),
        Button(glm::vec2(170, 300), glm::vec2(345, 74), "Creditos"),
        Button(glm::vec2(170, 200), glm::vec2(215, 74), "Salir")
    };

    // Boton de Ayuda y Creditos
    Button botonAtras = Button(glm::vec2(20, 1000), glm::vec2(235, 75), "Atras");

    // Estado del menu
    AppState estadoActual = MENU_PRINCIPAL;
    ButtonClick clickState;
    bool mousePressed = false;
    bool mouseReleased = false;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // L�gica de las nubes
    bool nubeArrastrando = false;
    glm::vec3 nubeOffset; // Por si quieres permitir un arrastre con desfase

    srand(static_cast<unsigned int>(time(0)));

    std::vector<glm::vec3> posicionesFijas = {
        glm::vec3(1.6f,  -0.8f, 4.0f),  // esquina superior izquierda
        glm::vec3(-1.6f,  -0.8f, 4.0f), // esquina superior derecha
        glm::vec3(1.6f, 0.8f, 4.0f),    // esquina inferior izquierda
        glm::vec3(-1.6f, 0.8f, 4.0f),   // esquina inferior derecha
        glm::vec3(0.0f,  0.1f, 4.0f),   // centro
        glm::vec3(1.6f,  0.1f, 4.0f),   // centro izquierda
        glm::vec3(0.0f,  -0.8f, 4.0f),  // centro arriba
        glm::vec3(0.0f, 0.8f, 4.0f)     // centro abajo
    };

    // Posici�n inicial y objetivo de la nube
    glm::vec3 nubePos = posicionesFijas[rand() % posicionesFijas.size()];
    glm::vec3 nubeTargetPos = posicionesFijas[rand() % posicionesFijas.size()];
    float nubeVelocidad = 0.05f;

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Camara y modelos
        modelShader.Activate();
        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        // Tiempo
        float time = glfwGetTime();
        float deltaTime = 0.016f; // Aproximadamente 60 FPS

        // Mouse
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

        if (estadoActual == MENU_PRINCIPAL) {

            // Fondo con textura
            glDisable(GL_DEPTH_TEST);
            backgroundShader.Activate();
            vaoF.Bind();
            backgroundMenu.Bind();
            backgroundMenu.texUnit(backgroundShader.ID, "backgroundTexture", 0);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glEnable(GL_DEPTH_TEST);

            // Nubes
            glm::vec3 direction = nubeTargetPos - nubePos;
            float distance = glm::length(direction);

            if (distance < 0.1f)
                nubeTargetPos = posicionesFijas[rand() % posicionesFijas.size()];

            if (distance > 0.0f)
                nubePos += glm::normalize(direction) * nubeVelocidad * deltaTime;

            glm::mat4 cloudTransform = glm::translate(glm::mat4(1.0f), nubePos);
            cloudTransform = glm::scale(cloudTransform, glm::vec3(0.5f));

            clouds.DrawRotation(modelShader, camera, cloudTransform);

            // Base y Tierra
            glDisable(GL_DEPTH_TEST);
            glm::mat4 ortho = glm::ortho(-1.0f * (float)width / height, 1.0f * (float)width / height, -1.0f, 1.0f);
            circleShader.Activate();
            glUniformMatrix4fv(glGetUniformLocation(circleShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(ortho));
            glUniform4f(glGetUniformLocation(circleShader.ID, "color"), 1.0f, 1.0f, 1.0f, 1.0f);
            glUniform2f(glGetUniformLocation(circleShader.ID, "scale"), 1.0f, 0.90f);
            circle.Draw(circleShader);
            glEnable(GL_DEPTH_TEST);

            glm::mat4 earthTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 3.7f));
            earthTransform = glm::rotate(earthTransform, glm::radians(time * 20.0f), glm::vec3(1.0f, 1.0f, 1.0f));
            earth.DrawRotation(modelShader, camera, earthTransform);

            // T�tulo
            glDisable(GL_DEPTH_TEST);
            textRenderer.RenderText(L"Wonder", 138.0f, height - 219.0f, 1.04f, glm::vec3(0.2f));
            textRenderer.RenderText(L"Scapes", 138.0f, height - 399.0f, 1.04f, glm::vec3(0.2f));
            textRenderer.RenderText(L"Wonder", 150.0f, height - 220.0f, 1.0f, glm::vec3(rgb(230), rgb(57), rgb(70)));
            textRenderer.RenderText(L"Scapes", 150.0f, height - 400.0f, 1.0f, glm::vec3(rgb(244), rgb(162), rgb(97)));
            glEnable(GL_DEPTH_TEST);

            // Logica de botones
            glDisable(GL_DEPTH_TEST);
            for (int i = 0; i < botonesMenu.size(); ++i) {
                bool hovered = botonesMenu[i].isHovered(mouseX, mouseY);
                bool clicked = false;
                botonesMenu[i].render(buttonRenderer, hovered, false);

                if (hovered && mousePressed && !clickState.waiting) {
                    clicked = true;
                    audio.playClickSound("media/click.mp3");
                    clickState.index = i;
                    clickState.clickTime = glfwGetTime();
                    clickState.waiting = true;
                }

                botonesMenu[i].render(buttonRenderer, hovered, (clickState.waiting == true && clickState.index == i) ? true : clicked);
            }
            glEnable(GL_DEPTH_TEST);

            if (clickState.waiting && (glfwGetTime() - clickState.clickTime) >= 0.3) {
                switch (clickState.index) {
                case 0: estadoActual = PANTALLA_EXPLORAR; break;
                case 1: estadoActual = PANTALLA_AYUDA; break;
                case 2: estadoActual = PANTALLA_CREDITOS; break;
                case 3: estadoActual = PANTALLA_SALIR; break;
                }
                clickState.waiting = false;
            }
        }
        else if (estadoActual == PANTALLA_AYUDA || estadoActual == PANTALLA_CREDITOS) {

            // Fondo con textura
            glDisable(GL_DEPTH_TEST);
            backgroundShader.Activate();
            vaoF.Bind();
            backgroundCH.Bind();
            backgroundCH.texUnit(backgroundShader.ID, "backgroundTexture", 0);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glEnable(GL_DEPTH_TEST);

            // Boton
            bool hovered = botonAtras.isHovered(mouseX, mouseY);
            bool clicked = false;
            glDisable(GL_DEPTH_TEST);
            botonAtras.render(buttonRenderer, hovered, clickState.waiting);
            glEnable(GL_DEPTH_TEST);

            if (hovered && mousePressed && !clickState.waiting) {
                clicked = true;
                audio.playClickSound("media/click.mp3");
                clickState.clickTime = glfwGetTime();
                clickState.waiting = true;
            }

            if (clickState.waiting && (glfwGetTime() - clickState.clickTime) >= 0.3) {
                estadoActual = MENU_PRINCIPAL;
                clickState.waiting = false;
            }

            if (estadoActual == PANTALLA_CREDITOS) {
                // Modelo del logo
                float angleDegrees = sin(time * 1.5f) * 25.0f;

                glm::vec3 uniPosition = glm::vec3(0.0f, -0.3f, 4.0f);
                glm::mat4 uniTransform = glm::translate(glm::mat4(1.0f), uniPosition);
                uniTransform = glm::rotate(uniTransform, glm::radians(angleDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
                uniTransform = glm::scale(uniTransform, glm::vec3(0.5f));
                uni.DrawRotation(modelShader, camera, uniTransform);

                glDisable(GL_DEPTH_TEST);
                creditsRenderer.RenderText(L"Universidad Nacional de Ingenieria", 270.0f, height - 195.0f, 1.0f, glm::vec3(0.0f));
                creditsRenderer.RenderText(L"Ingenieria en Computacion", 435.0f, height - 295.0f, 1.0f, glm::vec3(0.0f));
                creditsRenderer.RenderText(L"Baltodano Rodriguez Carles David    2023-0663U", 245.0f, height - 645.0f, 0.75f, glm::vec3(0.0f));
                creditsRenderer.RenderText(L"Guerrero Madrigal Joan Ulises           2023-0618U", 245.0f, height - 725.0f, 0.75f, glm::vec3(0.0f));
                creditsRenderer.RenderText(L"Hernandez Martinez Julio Cesar        2023-0595U", 245.0f, height - 805.0f, 0.75f, glm::vec3(0.0f));
                creditsRenderer.RenderText(L"Rodriguez Diaz Axel Josue                    2023-0758U", 245.0f, height - 885.0f, 0.75f, glm::vec3(0.0f));
                // creditsRenderer.RenderText(L"Hola", 1675.0f, height - 600.0f, 1.0f, glm::vec3(0.0f));
                glEnable(GL_DEPTH_TEST);
            }
            else if (estadoActual == PANTALLA_AYUDA) {

            }
        }
        else if (estadoActual == PANTALLA_EXPLORAR) {
            audio.stopBackgroundMusic();
            UI_Tierra(window, modelShader, earth, lupa, animation);
            estadoActual = MENU_PRINCIPAL;
            audio.playBackgroundMusic("media/environment.mp3");
        }
        else if (estadoActual == PANTALLA_SALIR) {
            break;
        }

        glEnable(GL_DEPTH_TEST);
        glfwSwapBuffers(window);
    }

    circle.Delete();
    vaoF.Delete(); vboF.Delete(); eboF.Delete();
    circleShader.Delete(); backgroundShader.Delete(); modelShader.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}