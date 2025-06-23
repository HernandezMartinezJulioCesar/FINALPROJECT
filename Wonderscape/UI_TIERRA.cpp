#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Config.h"
#include "UI_TIERRA.h"
#include"SceneModels.h"
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

std::vector<std::string> nombreMaravillas = {
    "PETRA",
    "TAJ MAHAL",
    "CHICHEN ITZA",
    "MACHU PICCHU",
    "ROMAN COLOSSEUM",
    "GREAT WALL OF CHINA",
    "CHRIST THE REDEEMER"
};

std::vector<std::string> direccionMaravillas = {
    "images/petra.jpg",
    "images/taj.jpg",
    "images/chichen.jpg",
    "images/machu.jpg",
    "images/coliseo.jpg",
    "images/wall.jpg",
    "images/cristo.jpg"
};

std::vector<glm::vec3> posicionCamara = {
    glm::vec3(-2.61041f, 1.9169f, 1.78921f),
    glm::vec3(-1.20549f, 1.69786f, 3.05844f),
    glm::vec3(-0.100319f, 1.33823f, -3.44805f),
    glm::vec3(-0.98453f, -0.707171f, -3.4958f),
    glm::vec3(-2.65715f, 2.51288f, 0.561229f),
    glm::vec3(1.18042f, 2.40114f, 2.55561f),
    glm::vec3(-2.33769f, -1.48195f, -2.45541f)
};

std::vector<glm::vec3> posicionLocation = {
    glm::vec3(0.84862f, -0.66570f, -0.58028f),
    glm::vec3(0.39097f, -0.57766f, -0.99193f),
    glm::vec3(0.01854f, -0.47502f, 1.11829f),
    glm::vec3(0.31231f, 0.18535f, 1.13377f),
    glm::vec3(0.86178f, -0.86699f, -0.18202f),
    glm::vec3(-0.38284f, -0.84575f, -0.82885f),
    glm::vec3(0.75817f, 0.42663f, 0.79635f)
};

std::vector<glm::vec3> rotacionLocation = {
    glm::vec3(-22.4f, -58.2997f, -14.1f),
    glm::vec3(-31.1f, -22.7f, -11.1f),
    glm::vec3(26.5f, 3.8f, -1.9f),
    glm::vec3(-10.5f, 12.2f, 0.8f),
    glm::vec3(-1.8f, -79.3994f, -1.1f),
    glm::vec3(-34.1f, 11.3f, 14.8f),
    glm::vec3(-11.2f, 41.8f, 8.9f)
};

float rgb1(int color) {
    return color / 255.0f;
}

std::wstring convertirAWideString(const std::string& str) {
    return std::wstring(str.begin(), str.end());
}

float pos(int point, float base) {
    return (point / base) * 2.0f - 1.0f;
}

void UI_Tierra(GLFWwindow* window, Shader& modelShader, Model& earth, Model& lupaModel, Model& animation, Model& model)
{   
    int maravilla = 0;
    // background audio
    AudioManager audio;
    audio.playBackgroundMusic("media/outer space.mp3");
    audio.setMusicVolume(0.1f);
    audio.setEffectsVolume(0.4f);

    // Load text
    TextRenderer textRenderer(width, height), buttonRenderer(width, height), principalRenderer(width, height);
    textRenderer.Load("fonts/Crushed.ttf", 55);
    buttonRenderer.Load("fonts/Crushed.ttf", 35);
    principalRenderer.Load("fonts/JungleAdventurer.ttf", 100);

    // Image for previews
    unsigned int Indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    // Image panel
    ShaderF panelShader("panel.vert", "panel.frag");

    float panelVertices[] = {
        pos(20, width), pos(820, height),
        pos(20, width), pos(260, height),
        pos(580, width), pos(260, height),
        pos(580, width), pos(820, height)
    };

    VAOF panelVAO;
    panelVAO.Bind();
    VBOF panelVBO(panelVertices, sizeof(panelVertices));
    EBOF panelEBO(Indices, sizeof(Indices));

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    panelVAO.Unbind();
    panelVBO.Unbind();
    panelEBO.Unbind();

    // Image
    ShaderF imageShader("background.vert", "background.frag");

    float imageVertices[] = {
        pos(60, width), pos(780, height), 0.0f, 1.0f,
        pos(60, width), pos(300, height), 0.0f, 0.0f,
        pos(540, width), pos(300, height), 1.0f, 0.0f,
        pos(540, width), pos(780, height), 1.0f, 1.0f
    };

    VAOF imagevaoF;
    imagevaoF.Bind();
    VBOF imagevboF(imageVertices, sizeof(imageVertices));
    EBOF imageeboF(Indices, sizeof(Indices));

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    imagevaoF.Unbind(); imagevboF.Unbind(); imageeboF.Unbind();
    TextureF previewImage(direccionMaravillas[0].c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);

    // Load localization model
    Model location("models/LocationIcon/scene.gltf", glm::vec3(1.0f));
    Texture Textura("models/LocationIcon/textures/rojo.jpg", "albedo", 0);
    for (auto& mesh : location.meshes)
    {
        mesh.textures.clear();
        mesh.textures.push_back(Textura);
    }

    // Botones de la pantalla Explore
    std::vector<Button> botonesExplore = {
        Button(glm::vec2(20, 780), glm::vec2(190, 55), nombreMaravillas[0]),
        Button(glm::vec2(20, 680), glm::vec2(320, 55), nombreMaravillas[1]),
        Button(glm::vec2(20, 580), glm::vec2(382, 55), nombreMaravillas[2]),
        Button(glm::vec2(20, 480), glm::vec2(410, 55), nombreMaravillas[3]),
        Button(glm::vec2(20, 380), glm::vec2(550, 55), nombreMaravillas[4]),
        Button(glm::vec2(20, 280), glm::vec2(615, 55), nombreMaravillas[5]),
        Button(glm::vec2(20, 180), glm::vec2(632, 55), nombreMaravillas[6]),
        Button(glm::vec2(20, 20), glm::vec2(200, 65), "Back"),
    };

    // Botones de la pantalla Vista Previa
    std::vector<Button> botonesPrevia = {
        Button(glm::vec2(1484, 436), glm::vec2(200, 74), "Back"),
        Button(glm::vec2(1429, 570), glm::vec2(310, 74), "Explore")
    };

    // Estado del panel
    AppState estadoActual = PANTALLA_EXPLORAR;
    ButtonClick clickState;
    bool mousePressed = false;
    bool mouseReleased = false;

    std::string titulo, rutaImagen;
    glm::vec3 posCamara = glm::vec3(0.0f);
    int maravillaSeleccionada = 0;

    // Camara orbital
    Camera camera(width, height, glm::vec3(0.0f));
    camera.Position = glm::vec3(0.0f, 0.0f, camera.orbitRadius);

    // Animacion inmersion
    bool animacionIniciada = false;
    float tiempoInicioAnimacion = 0.0f;
    float lastFrame = 0.0f;

    // Tierra
    float duracionTierra = 2.0f;
    float escalaTierraInicial = 1.0f;
    float escalaTierraFinal = 3.6f;
    float progresoTierra = escalaTierraInicial;

    // Nubes
    float duracionNubes = 1.0f;
    float escalaNubesInicial = 0.007f;
    float escalaNubesFinal = 0.009f;
    float progresoNube = escalaNubesInicial;

    while (true)
    {
        // Detectar si se presion� ESC para volver al men�
        glfwPollEvents();

        // Tiempo
        float time = glfwGetTime();
        float deltaTime = time - lastFrame;  // Tiempo real entre frames
        lastFrame = time;

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

        // T�tulo
        if (estadoActual != PANTALLA_VIAJAR) {
            glDisable(GL_DEPTH_TEST);
            textRenderer.RenderText(L"THE 7 WONDERS OF THE MODERN WEST", 110.0f, height - 120.0f, 1.0f, glm::vec3(1.0f));
            glEnable(GL_DEPTH_TEST);
        }

        if (estadoActual == PANTALLA_EXPLORAR) {
            camera.inputsEnabled = true;

            // Botones de maravillas
            glDisable(GL_DEPTH_TEST);
            for (int i = 0; i < botonesExplore.size(); ++i) {
                if (i != 7) botonesExplore[i].position.y -= 12;
                bool hovered = botonesExplore[i].isHovered(mouseX, mouseY);
                if (i != 7) botonesExplore[i].position.y += 12;
                bool clicked = false;
                botonesExplore[i].render(((i == 7) ? principalRenderer : buttonRenderer), hovered, false);

                if (hovered) {
                    camera.inputsEnabled = false;

                    if (mousePressed && !clickState.waiting) {
                        clicked = true;
                        audio.playClickSound("media/click.mp3");
                        clickState.index = i;
                        clickState.clickTime = glfwGetTime();
                        clickState.waiting = true;
                    }
                }

                botonesExplore[i].render(((i == 7) ? principalRenderer : buttonRenderer), hovered, (clickState.waiting == true && clickState.index == i) ? true : clicked);
            }
            glEnable(GL_DEPTH_TEST);

            if (clickState.waiting && (glfwGetTime() - clickState.clickTime) >= 0.3) {
                camera.inputsEnabled = false;

                switch (clickState.index) {
                case 0: titulo = nombreMaravillas[0]; rutaImagen = direccionMaravillas[0]; posCamara = posicionCamara[0]; maravilla = 2; break;
                case 1: titulo = nombreMaravillas[1]; rutaImagen = direccionMaravillas[1]; posCamara = posicionCamara[1]; maravilla = 7; break;
                case 2: titulo = nombreMaravillas[2]; rutaImagen = direccionMaravillas[2]; posCamara = posicionCamara[2]; maravilla = 5; break;
                case 3: titulo = nombreMaravillas[3]; rutaImagen = direccionMaravillas[3]; posCamara = posicionCamara[3]; maravilla = 4; break;
                case 4: titulo = nombreMaravillas[4]; rutaImagen = direccionMaravillas[4]; posCamara = posicionCamara[4]; maravilla = 6; break;
                case 5: titulo = nombreMaravillas[5]; rutaImagen = direccionMaravillas[5]; posCamara = posicionCamara[5]; maravilla = 1; break;
                case 6: titulo = nombreMaravillas[6]; rutaImagen = direccionMaravillas[6]; posCamara = posicionCamara[6]; maravilla = 3; break;
                }
                clickState.waiting = false;

                if (clickState.index == 7) break;

                previewImage = TextureF(rutaImagen.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
                estadoActual = PANTALLA_VISTA_PREVIA;
                maravillaSeleccionada = clickState.index;
            }

            // Dibujar Tierra
            glm::mat4 earthTransform = glm::mat4(1.0f);
            earthTransform = glm::rotate(earthTransform, glm::radians(time * 20.0f), glm::vec3(0.0f, 0.8f, 0.1f));
            earthTransform = glm::translate(earthTransform, glm::vec3(0.0f));
            earthTransform = glm::scale(earthTransform, glm::vec3(1.0f));
            earth.DrawRotation(modelShader, camera, earthTransform);

            // Actualizar camara
            camera.OrbitInputs(window);
            camera.updateMatrixExplore(45.0f, 0.1f, 100.0f);
        }
        else if (estadoActual == PANTALLA_VISTA_PREVIA) {
            // Actualizar camara
            camera.Position = posCamara;
            camera.OrbitInputs(window);
            camera.updateMatrixExplore(45.0f, 0.1f, 100.0f);

            // Dibujar Tierra
            modelShader.Activate();
            glm::mat4 earthTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
            earth.DrawRotation(modelShader, camera, earthTransform);

            // Dibujar marcador de ubicaci�n en la Tierra
            glm::mat4 locationTransform = glm::translate(glm::mat4(1.0f), posicionLocation[maravillaSeleccionada]);

            locationTransform = glm::rotate(locationTransform, glm::radians(rotacionLocation[maravillaSeleccionada].x), glm::vec3(1.0f, 0.0f, 0.0f));
            locationTransform = glm::rotate(locationTransform, glm::radians(rotacionLocation[maravillaSeleccionada].y), glm::vec3(0.0f, 1.0f, 0.0f));
            locationTransform = glm::rotate(locationTransform, glm::radians(rotacionLocation[maravillaSeleccionada].z), glm::vec3(0.0f, 0.0f, 1.0f));

            locationTransform = glm::scale(locationTransform, glm::vec3(0.025f));
            location.DrawRotation(modelShader, camera, locationTransform);

            // Dibujar t�tulo
            float here = 1248.0f + ((672.0f - botonesExplore[maravillaSeleccionada].size.x) / 2);
            glDisable(GL_DEPTH_TEST);
            buttonRenderer.RenderText(convertirAWideString(titulo), here, 770.0f, 1.0f, glm::vec3(1.0f));
            glEnable(GL_DEPTH_TEST);

            // Logica de botones
            glDisable(GL_DEPTH_TEST);
            for (int i = 0; i < botonesPrevia.size(); ++i) {
                bool hovered = botonesPrevia[i].isHovered(mouseX, mouseY);
                bool clicked = false;
                botonesPrevia[i].render(principalRenderer, hovered, false);

                if (hovered && mousePressed && !clickState.waiting) {
                    clicked = true;
                    audio.playClickSound((i == 0) ? "media/click.mp3" : "media/clickViaje.mp3");
                    clickState.index = i;
                    clickState.clickTime = glfwGetTime();
                    clickState.waiting = true;
                }

                botonesPrevia[i].render(principalRenderer, hovered, (clickState.waiting == true && clickState.index == i) ? true : clicked);
            }
            glEnable(GL_DEPTH_TEST);

            if (clickState.waiting && (glfwGetTime() - clickState.clickTime) >= 0.3) {
                switch (clickState.index) {
                case 0: estadoActual = PANTALLA_EXPLORAR; break;
                case 1: estadoActual = PANTALLA_VIAJAR; break;
                }
                camera = Camera(width, height, glm::vec3(0.0f));
                camera.Position = glm::vec3(0.0f, 0.0f, camera.orbitRadius);
                clickState.waiting = false;
            }

            // Dibujar panel de fondo (transparente) detr�s de la imagen
            glDisable(GL_DEPTH_TEST);
            panelShader.Activate();
            panelVAO.Bind();
            glUniform4f(glGetUniformLocation(panelShader.ID, "panelColor"), 0.0f, 0.0f, 0.0f, 0.55f);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // Dibujar imagen de la maravilla
            imageShader.Activate();
            imagevaoF.Bind();
            previewImage.Bind();
            previewImage.texUnit(imageShader.ID, "backgroundTexture", 0);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glEnable(GL_DEPTH_TEST);
        }
        else if (estadoActual == PANTALLA_VIAJAR) {
            if (!animacionIniciada) {
                animacionIniciada = true;
                tiempoInicioAnimacion = glfwGetTime();
                progresoTierra = escalaTierraInicial;
                progresoNube = escalaNubesInicial;
            }

            // Actualizar camara
            camera.Position = posCamara;
            camera.OrbitInputs(window);
            camera.updateMatrixExplore(45.0f, 0.1f, 100.0f);

            float tiempoActual = glfwGetTime() - tiempoInicioAnimacion;

            // Animacion de la Tierra
            if (tiempoActual <= duracionTierra) {
                progresoTierra = escalaTierraInicial + (escalaTierraFinal - escalaTierraInicial) * (tiempoActual / duracionTierra);
                glm::mat4 earthTransform = glm::mat4(1.0f);
                earthTransform = glm::scale(earthTransform, glm::vec3(progresoTierra));
                earth.DrawRotation(modelShader, camera, earthTransform);
            }

            // Animacion de las nubes
            if (tiempoActual > duracionTierra && tiempoActual < (duracionTierra + duracionNubes)) {
                float tiempoNubes = tiempoActual - duracionTierra;
                progresoNube = escalaNubesInicial + (escalaNubesFinal - escalaNubesInicial) * (tiempoNubes / duracionNubes);
                glm::mat4 animationTransform = glm::mat4(1.0f);
                animationTransform = glm::scale(animationTransform, glm::vec3(progresoNube));
                animation.DrawRotation(modelShader, camera, animationTransform);
            }

            // Finalizar animacion despues de los segundos
            if (tiempoActual >= duracionTierra + duracionNubes) {
                audio.stopBackgroundMusic();
                PantallaCarga(window, modelShader, lupaModel, 5.0f);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                if (SceneModels(maravilla, window, model)) {
                    animacionIniciada = false;
                    estadoActual = PANTALLA_EXPLORAR;
                    camera = Camera(width, height, glm::vec3(0.0f));
                    camera.Position = glm::vec3(0.0f, 0.0f, camera.orbitRadius);
                    audio.playBackgroundMusic("media/outer space.mp3");
                }
            }
        }

        glEnable(GL_DEPTH_TEST);
        glfwSwapBuffers(window);
    }

    audio.stopBackgroundMusic();
}