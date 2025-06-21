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
#include "Texture.h"
#include "TextureF.h"
#include "VAOF.h"
#include "VBOF.h"
#include "EBOF.h"

float rgb1(int color) {
    return color / 255.0f;
}

std::wstring convertirAWideString(const std::string& str) {
    return std::wstring(str.begin(), str.end());
}

AppState mostrarVistaPrevia(GLFWwindow* window, Camera& camera, Shader& modelShader, Model& earth,
    TextRenderer& textRenderer, ShaderF& backgroundShader, VAOF& vaoF,
    AudioManager& audio, const std::string& titulo, const std::string& imagenPath, glm::vec3 rotacionFija)
{
    // Imagen de fondo
    TextureF previewImage(imagenPath.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);

    // Botones
    Button botonExplorar(glm::vec2(750, 20), glm::vec2(200, 60), "Explore");
    Button botonAtras(glm::vec2(20, 20), glm::vec2(200, 60), "Back");

    ButtonClick clickState, clickBack;
    bool mousePressed = false;

    // Cargar modelo de localización
    Model location("models/LocationIcon/scene.gltf");
    Texture Textura("models/LocationIcon/textures/rojo.jpg", "albedo", 0);
    for (auto& mesh : location.meshes)
    {
        mesh.textures.clear();
        mesh.textures.push_back(Textura);
    }

    // Loop de renderizado
    while (!glfwWindowShouldClose(window)) {
        float deltaTime = 0.016f;
        glfwPollEvents();

        // Entrada de mouse
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        mouseY = height - mouseY;
        mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

        // Limpiar pantalla
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Actualizar cámara
        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        // Dibujar Tierra orientada a la maravilla
        glm::mat4 earthTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.2f, 3.7f));
        earthTransform = glm::rotate(earthTransform, rotacionFija.x, glm::vec3(1.0f, 0.0f, 0.0f));
        earthTransform = glm::rotate(earthTransform, rotacionFija.y, glm::vec3(0.0f, 1.0f, 0.0f));
        earthTransform = glm::rotate(earthTransform, rotacionFija.z, glm::vec3(0.0f, 0.0f, 1.0f));
        earth.DrawRotation(modelShader, camera, earthTransform);

        // Dibujar marcador de ubicación en la Tierra
        glm::mat4 locationTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-0.3f, -0.25f, 1.0f));
        locationTransform = glm::scale(locationTransform, glm::vec3(0.025f)); // tamaño pequeño
        modelShader.Activate();
        location.DrawRotation(modelShader, camera, locationTransform);

        // Dibujar imagen de fondo
        glDisable(GL_DEPTH_TEST);
        backgroundShader.Activate();
        vaoF.Bind();
        previewImage.Bind();
        previewImage.texUnit(backgroundShader.ID, "backgroundTexture", 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glEnable(GL_DEPTH_TEST);

        // Dibujar título
        glDisable(GL_DEPTH_TEST);
        textRenderer.RenderText(convertirAWideString(titulo), 500.0f, height - 100.0f, 1.0f, glm::vec3(rgb1(230), rgb1(57), rgb1(70)));
        glEnable(GL_DEPTH_TEST);

        // Botón "Explore"
        bool hoveredExplore = botonExplorar.isHovered(mouseX, mouseY);
        botonExplorar.render(textRenderer, height, hoveredExplore, false);
        if (hoveredExplore && mousePressed && !clickState.waiting) {
            audio.playClickSound();
            clickState.clickTime = glfwGetTime();
            clickState.waiting = true;
        }
        if (clickState.waiting && (glfwGetTime() - clickState.clickTime >= 0.3)) {
            clickState.waiting = false;
            return PANTALLA_EXPLORAR;
        }

        // Botón "Back"
        bool hoveredBack = botonAtras.isHovered(mouseX, mouseY);
        botonAtras.render(textRenderer, height, hoveredBack, false);
        if (hoveredBack && mousePressed && !clickBack.waiting) {
            audio.playClickSound();
            clickBack.clickTime = glfwGetTime();
            clickBack.waiting = true;
        }
        if (clickBack.waiting && (glfwGetTime() - clickBack.clickTime >= 0.3)) {
            clickBack.waiting = false;
            return PANTALLA_EXPLORAR;
        }

        // Mostrar frame
        glfwSwapBuffers(window);
    }

    return PANTALLA_EXPLORAR;
}


void UI_Tierra(GLFWwindow* window, Camera& camera, Shader& modelShader, Model& earth, TextRenderer& buttonRenderer, AudioManager& audio)
{
    // Estado del panel
    AppState estadoActual = PANTALLA_EXPLORAR, siguientePantalla = MENU_PRINCIPAL;

    // Boton de click
    ButtonClick clickState;
    bool mousePressed = false;
    bool mouseReleased = false;

    // Botones del Menu Principal
    std::vector<Button> botonesMaravillas = {
        Button(glm::vec2(20, 700), glm::vec2(850, 75), "Great Wall of China"),
        Button(glm::vec2(20, 600), glm::vec2(230, 75), "Petra"),
        Button(glm::vec2(20, 500), glm::vec2(840, 75), "Christ the Redeemer"),
        Button(glm::vec2(20, 400), glm::vec2(600, 75), "Machu Picchu"),
        Button(glm::vec2(20, 300), glm::vec2(520, 75), "Chichen Itza"),
        Button(glm::vec2(20, 200), glm::vec2(730, 75), "Roman Colosseum"),
        Button(glm::vec2(20, 100), glm::vec2(430, 75), "Taj Mahal")
    };

    ShaderF backgroundShader("background.vert", "background.frag");

    float backgroundVertices[] = {
    -0.8333f,  0.7407f,  0.0f, 1.0f,
    -0.8333f, -0.7407f,  0.0f, 0.0f,
    -0.1667f, -0.7407f,  1.0f, 0.0f,
    -0.1667f,  0.7407f,  1.0f, 1.0f
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

    vaoF.Unbind();


    // Cargar texto
    TextRenderer textRenderer(width, height);
    textRenderer.Load("fonts/River Adventurer.ttf", 100);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    while (true)
    {
        // Detectar si se presionó ESC para volver al menú
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            estadoActual = MENU_PRINCIPAL;
            break;
        }

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

        float normalizedX = (mouseX / width) * 2.0f - 1.0f;
        float normalizedY = (mouseY / height) * 2.0f - 1.0f;
        glm::vec3 mouseWorldPos = glm::vec3(-normalizedX * (float)width / height * 3.0f, -normalizedY * 3.0f, 1.0f);

        // Limpiar pantalla
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Título
        glDisable(GL_DEPTH_TEST);
        textRenderer.RenderText(L"The 7 Wonders of", 88.0f, height - 219.0f + 100.0f, 1.04f, glm::vec3(0.2f));
        textRenderer.RenderText(L"the Modern World", 88.0f, height - 399.0f + 160.0f, 1.04f, glm::vec3(0.2f));
        textRenderer.RenderText(L"The 7 Wonders of", 100.0f, height - 220.0f + 100.0f, 1.0f, glm::vec3(rgb1(230), rgb1(57), rgb1(70)));
        textRenderer.RenderText(L"the Modern World", 100.0f, height - 400.0f + 160.0f, 1.0f, glm::vec3(rgb1(244), rgb1(162), rgb1(97)));
        glEnable(GL_DEPTH_TEST);

        // Botones de maravillas
        glDisable(GL_DEPTH_TEST);
        for (int i = 0; i < botonesMaravillas.size(); ++i) {
            bool hovered = botonesMaravillas[i].isHovered(mouseX, mouseY);
            bool clicked = false;
            botonesMaravillas[i].render(buttonRenderer, height, hovered, false);

            if (hovered && mousePressed && !clickState.waiting) {
                clicked = true;
                audio.playClickSound();
                clickState.index = i;
                clickState.clickTime = glfwGetTime();
                clickState.waiting = true;
            }

            botonesMaravillas[i].render(buttonRenderer, height, hovered, (clickState.waiting == true && clickState.index == i) ? true : clicked);
        }
        glEnable(GL_DEPTH_TEST);

        if (clickState.waiting) {
            double elapsed = glfwGetTime() - clickState.clickTime;

            if (elapsed >= 0.3) {
                std::string titulo, rutaImagen;
                glm::vec3 rotacionMaravilla;

                switch (clickState.index) {
                case 0: titulo = "Great Wall of China"; rutaImagen = "images/wall.jpg"; rotacionMaravilla = glm::vec3(0.0f, glm::radians(-30.0f), 0.0f); break;
                case 1: titulo = "Petra"; rutaImagen = "images/petra.jpg"; rotacionMaravilla = glm::vec3(0.0f, glm::radians(70.0f), 0.0f); break;
                case 2: titulo = "Christ the Redeemer"; rutaImagen = "images/cristo.jpg"; rotacionMaravilla = glm::vec3(0.0f, glm::radians(120.0f), 0.0f); break;
                case 3: titulo = "Machu Picchu"; rutaImagen = "images/machu.jpg"; rotacionMaravilla = glm::vec3(0.0f, glm::radians(150.0f), 0.0f); break;
                case 4: titulo = "Chichen Itza"; rutaImagen = "images/chichen.jpg"; rotacionMaravilla = glm::vec3(0.0f, glm::radians(170.0f), 0.0f); break;
                case 5: titulo = "Roman Colosseum"; rutaImagen = "images/coliseo.jpg"; rotacionMaravilla = glm::vec3(0.0f, glm::radians(15.0f), 0.0f); break;
                case 6: titulo = "Taj Mahal"; rutaImagen = "images/taj.jpg"; rotacionMaravilla = glm::vec3(0.0f, glm::radians(0.0f), 0.0f); break;
                }

                AppState nuevoEstado = mostrarVistaPrevia(window, camera, modelShader, earth, textRenderer, backgroundShader, vaoF, audio, titulo, rutaImagen, rotacionMaravilla);
                clickState.waiting = false;
                if (nuevoEstado == MENU_PRINCIPAL) {
                    estadoActual = MENU_PRINCIPAL;
                    break;
                }
            }


        }

        // Habilitar entrada de cámara cuando no se está sobre un botón
        bool estaSobreUnBoton = false;
        for (auto& boton : botonesMaravillas) {
            if (boton.isHovered(mouseX, mouseY)) {
                estaSobreUnBoton = true;
                break;
            }
        }

        camera.inputsEnabled = !estaSobreUnBoton;

        // Actualizar cámara
        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        // Dibujar Tierra y ubicación
        glm::mat4 earthTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.2f, 3.7f));
        earthTransform = glm::rotate(earthTransform, glm::radians(time * 20.0f), glm::vec3(0.1f, 0.8f, 0.0f));
        earth.DrawRotation(modelShader, camera, earthTransform);

        glfwSwapBuffers(window);
    }
}