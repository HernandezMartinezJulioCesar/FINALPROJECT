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

    // Cargar icono de la ventana
    int iconWidth, iconHeight, channels;
    unsigned char* iconPixels = stbi_load("images/icono.png", &iconWidth, &iconHeight, &channels, 4);
    if (iconPixels) {
        GLFWimage icon;
        icon.width = iconWidth;
        icon.height = iconHeight;
        icon.pixels = iconPixels;
        glfwSetWindowIcon(window, 1, &icon);
        stbi_image_free(iconPixels);
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

    TextureF backgroundMenu("images/backgrounds/menu.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
    TextureF backgroundHelp("images/backgrounds/help.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
    TextureF backgroundCredits("images/backgrounds/credits.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);

    // Circulo base para la Tierra
    ShaderF circleShader("circle.vert", "circle.frag");
    Circle circle(1.38f, 0.0f, 0.80f);

    // Cargar modelos de la Tierra y las Nubes
    Shader modelShader("default.vert", "defaultModel.frag");
    glm::vec4 lightColor = glm::vec4(1.0f);

    modelShader.Activate();
    glUniform4f(glGetUniformLocation(modelShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);

    Camera camera(width, height, glm::vec3(0.0f));
    Model earth("models/tierra/scene.gltf", glm::vec3(1.0f));
    Model clouds("models/nubes/scene.gltf", glm::vec3(1.0f));
    Model uni("models/ModelUni/UNI.gltf", glm::vec3(1.0f));
    Model glass("models/MagnifyingGlass/scene.gltf", glm::vec3(1.0f));
    Model animation("models/LoadingScreen/scene.gltf", glm::vec3(1.0f));
    Model model("models/maravillas/scene.gltf", glm::vec3(0.45f));

    Texture texture("models/ModelUni/textures/Image_0.jpg", "albedo", 0);
    for (auto& mesh : uni.meshes) {
        mesh.textures.clear();
        mesh.textures.push_back(texture);
    }
    texture = Texture("models/MagnifyingGlass/texture/Image_0.jpg", "albedo", 0);
    for (auto& mesh : glass.meshes) {
        mesh.textures.clear();
        mesh.textures.push_back(texture);
    }

    // Botones del Menu Principal
    std::vector<Button> botonsMenu = {
        Button(glm::vec2(170, 500), glm::vec2(310, 74), "Explore"),
        Button(glm::vec2(170, 400), glm::vec2(180, 74), "Help"),
        Button(glm::vec2(170, 300), glm::vec2(295, 74), "Credits"),
        Button(glm::vec2(170, 200), glm::vec2(160, 74), "Exit")
    };

    // Boton de Ayuda y Creditos
    Button backButton = Button(glm::vec2(20, 1000), glm::vec2(200, 75), "Back");

    // Estado del menu
    AppState currentStatus = MENU_PRINCIPAL;
    ButtonClick clickState;
    bool mousePressed = false;
    bool mouseReleased = false;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Logica de las nubes
    srand(static_cast<unsigned int>(time(0)));

    std::vector<glm::vec3> fixedPositions = {
        glm::vec3(1.6f,  -0.8f, 4.0f),  // esquina superior izquierda
        glm::vec3(-1.6f,  -0.8f, 4.0f), // esquina superior derecha
        glm::vec3(1.6f, 0.8f, 4.0f),    // esquina inferior izquierda
        glm::vec3(-1.6f, 0.8f, 4.0f),   // esquina inferior derecha
        glm::vec3(0.0f,  0.1f, 4.0f),   // centro
        glm::vec3(1.6f,  0.1f, 4.0f),   // centro izquierda
        glm::vec3(0.0f,  -0.8f, 4.0f),  // centro arriba
        glm::vec3(0.0f, 0.8f, 4.0f)     // centro abajo
    };

    // Posicion inicial y objetivo de la nube
    glm::vec3 cloudPosition = fixedPositions[rand() % fixedPositions.size()];
    glm::vec3 cloudEndPosition = fixedPositions[rand() % fixedPositions.size()];
    float cloudSpeed = 0.05f;

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Camara y modelos
        modelShader.Activate();
        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        // Tiempo
        float time = glfwGetTime();
        float deltaTime = 0.016f;

        // Mouse
        static bool wasMousePressed = false;
        int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        mousePressed = (state == GLFW_PRESS && !wasMousePressed);
        mouseReleased = (state == GLFW_RELEASE && wasMousePressed);
        wasMousePressed = (state == GLFW_PRESS);

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        mouseY = height - mouseY;

        if (currentStatus == MENU_PRINCIPAL) {

            // Fondo con textura
            glDisable(GL_DEPTH_TEST);
            backgroundShader.Activate();
            vaoF.Bind();
            backgroundMenu.Bind();
            backgroundMenu.texUnit(backgroundShader.ID, "backgroundTexture", 0);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glEnable(GL_DEPTH_TEST);

            // Nubes
            glm::vec3 direction = cloudEndPosition - cloudPosition;
            float distance = glm::length(direction);

            if (distance < 0.1f)
                cloudEndPosition = fixedPositions[rand() % fixedPositions.size()];

            if (distance > 0.0f)
                cloudPosition += glm::normalize(direction) * cloudSpeed * deltaTime;

            glm::mat4 cloudTransform = glm::translate(glm::mat4(1.0f), cloudPosition);
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

            // Titulo
            glDisable(GL_DEPTH_TEST);
            textRenderer.RenderText(L"Wonder", 138.0f, height - 219.0f, 1.04f, glm::vec3(0.2f));
            textRenderer.RenderText(L"Scapes", 138.0f, height - 399.0f, 1.04f, glm::vec3(0.2f));
            textRenderer.RenderText(L"Wonder", 150.0f, height - 220.0f, 1.0f, glm::vec3(rgb(230), rgb(57), rgb(70)));
            textRenderer.RenderText(L"Scapes", 150.0f, height - 400.0f, 1.0f, glm::vec3(rgb(244), rgb(162), rgb(97)));
            glEnable(GL_DEPTH_TEST);

            // Logica de botones
            glDisable(GL_DEPTH_TEST);
            for (int i = 0; i < botonsMenu.size(); ++i) {
                bool hovered = botonsMenu[i].isHovered(mouseX, mouseY);
                bool clicked = false;
                botonsMenu[i].render(buttonRenderer, hovered, false);

                if (hovered && mousePressed && !clickState.waiting) {
                    clicked = true;
                    audio.playClickSound("media/click.mp3");
                    clickState.index = i;
                    clickState.clickTime = glfwGetTime();
                    clickState.waiting = true;
                }

                botonsMenu[i].render(buttonRenderer, hovered, (clickState.waiting == true && clickState.index == i) ? true : clicked);
            }
            glEnable(GL_DEPTH_TEST);

            if (clickState.waiting && (glfwGetTime() - clickState.clickTime) >= 0.3) {
                switch (clickState.index) {
                case 0: currentStatus = PANTALLA_EXPLORAR; break;
                case 1: currentStatus = PANTALLA_AYUDA; break;
                case 2: currentStatus = PANTALLA_CREDITOS; break;
                case 3: currentStatus = PANTALLA_SALIR; break;
                }
                clickState.waiting = false;
            }
        }
        else if (currentStatus == PANTALLA_AYUDA || currentStatus == PANTALLA_CREDITOS) {

            if (currentStatus == PANTALLA_AYUDA) {
                // Fondo con textura
                glDisable(GL_DEPTH_TEST);
                backgroundShader.Activate();
                vaoF.Bind();
                backgroundHelp.Bind();
                backgroundHelp.texUnit(backgroundShader.ID, "backgroundTexture", 0);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glEnable(GL_DEPTH_TEST);
            }
            else if (currentStatus == PANTALLA_CREDITOS) {
                // Fondo con textura
                glDisable(GL_DEPTH_TEST);
                backgroundShader.Activate();
                vaoF.Bind();
                backgroundCredits.Bind();
                backgroundCredits.texUnit(backgroundShader.ID, "backgroundTexture", 0);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glEnable(GL_DEPTH_TEST);

                // Modelo del logo
                float angleDegrees = sin(time * 1.5f) * 25.0f;
                glm::mat4 uniTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, 0.3f, 5.0f));
                uniTransform = glm::rotate(uniTransform, glm::radians(angleDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
                uniTransform = glm::scale(uniTransform, glm::vec3(0.5f));
                uni.DrawRotation(modelShader, camera, uniTransform);
            }

            // Boton
            bool hovered = backButton.isHovered(mouseX, mouseY);
            bool clicked = false;
            glDisable(GL_DEPTH_TEST);
            backButton.render(buttonRenderer, hovered, clickState.waiting);
            glEnable(GL_DEPTH_TEST);

            if (hovered && mousePressed && !clickState.waiting) {
                clicked = true;
                audio.playClickSound("media/click.mp3");
                clickState.clickTime = glfwGetTime();
                clickState.waiting = true;
            }

            if (clickState.waiting && (glfwGetTime() - clickState.clickTime) >= 0.3) {
                currentStatus = MENU_PRINCIPAL;
                clickState.waiting = false;
            }
        }
        else if (currentStatus == PANTALLA_EXPLORAR) {
            audio.stopBackgroundMusic();
            UI_Tierra(window, modelShader, earth, glass, animation, model);
            currentStatus = MENU_PRINCIPAL;
            audio.playBackgroundMusic("media/environment.mp3");
        }
        else if (currentStatus == PANTALLA_SALIR) {
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