#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "WondersMenu.h"
#include "SceneModels.h"

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

std::vector<std::string> nameWonders = {
    "PETRA",
    "TAJ MAHAL",
    "CHICHEN ITZA",
    "MACHU PICCHU",
    "ROMAN COLOSSEUM",
    "GREAT WALL OF CHINA",
    "CHRIST THE REDDEMER"
};

std::vector<std::string> routesWonders = {
    "images/preview/petra.jpg",
    "images/preview/taj.jpg",
    "images/preview/chichen.jpg",
    "images/preview/machu.jpg",
    "images/preview/coliseo.jpg",
    "images/preview/wall.jpg",
    "images/preview/cristo.jpg"
};

std::vector<glm::vec3> cameraPosition = {
    glm::vec3(-2.61041f, 1.9169f, 1.78921f),
    glm::vec3(-1.20549f, 1.69786f, 3.05844f),
    glm::vec3(-0.100319f, 1.33823f, -3.44805f),
    glm::vec3(-0.98453f, -0.707171f, -3.4958f),
    glm::vec3(-2.65715f, 2.51288f, 0.561229f),
    glm::vec3(1.18042f, 2.40114f, 2.55561f),
    glm::vec3(-2.33769f, -1.48195f, -2.45541f)
};

std::vector<glm::vec3> indicatorPosition = {
    glm::vec3(0.84862f, -0.66570f, -0.58028f),
    glm::vec3(0.39097f, -0.57766f, -0.99193f),
    glm::vec3(0.01854f, -0.47502f, 1.11829f),
    glm::vec3(0.31231f, 0.18535f, 1.13377f),
    glm::vec3(0.86178f, -0.86699f, -0.18202f),
    glm::vec3(-0.38284f, -0.84575f, -0.82885f),
    glm::vec3(0.75817f, 0.42663f, 0.79635f)
};

std::vector<glm::vec3> indicatorRotation = {
    glm::vec3(-22.4f, -58.2997f, -14.1f),
    glm::vec3(-31.1f, -22.7f, -11.1f),
    glm::vec3(26.5f, 3.8f, -1.9f),
    glm::vec3(-10.5f, 12.2f, 0.8f),
    glm::vec3(-1.8f, -79.3994f, -1.1f),
    glm::vec3(-34.1f, 11.3f, 14.8f),
    glm::vec3(-11.2f, 41.8f, 8.9f)
};

void WondersMenu(GLFWwindow* window, Shader& modelShader, Model& earth, Model& glassModel, Model& animation, Model& model) {
    // Load audio
    AudioManager audio;
    audio.playBackgroundMusic("media/outerSpace.mp3");
    audio.setMusicVolume(0.05f);
    audio.setEffectsVolume(0.4f);

    // Load text
    TextRenderer textRenderer(width, height), titleRenderer(width, height), buttonRenderer(width, height), principalRenderer(width, height);
    textRenderer.Load("fonts/Crushed.ttf", 55);
    titleRenderer.Load("fonts/Crushed.ttf", 35);
    buttonRenderer.Load("fonts/JungleAdventurer.ttf", 70);
    principalRenderer.Load("fonts/JungleAdventurer.ttf", 100);

    // Load background
    ShaderF imageShader("background.vert", "background.frag");

    float backgroundVertices[] = {
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
    };
    unsigned int Indexes[] = {
        0, 1, 2,
        2, 3, 0
    };

    VAOF vaoF;
    vaoF.Bind();
    VBOF vboF(backgroundVertices, sizeof(backgroundVertices));
    EBOF eboF(Indexes, sizeof(Indexes));

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    vaoF.Unbind(); vboF.Unbind(); eboF.Unbind();
    TextureF backgroundExplore("images/backgrounds/prueba.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);

    // Load panel
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
    EBOF panelEBO(Indexes, sizeof(Indexes));

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    panelVAO.Unbind();
    panelVBO.Unbind();
    panelEBO.Unbind();

    // Load image
    float imageVertices[] = {
        pos(60, width), pos(780, height), 0.0f, 1.0f,
        pos(60, width), pos(300, height), 0.0f, 0.0f,
        pos(540, width), pos(300, height), 1.0f, 0.0f,
        pos(540, width), pos(780, height), 1.0f, 1.0f
    };

    VAOF imagevaoF;
    imagevaoF.Bind();
    VBOF imagevboF(imageVertices, sizeof(imageVertices));
    EBOF imageeboF(Indexes, sizeof(Indexes));

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    imagevaoF.Unbind(); imagevboF.Unbind(); imageeboF.Unbind();
    TextureF previewImage(routesWonders[0].c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);

    // Load indicator model
    Model location("models/LocationIcon/scene.gltf", glm::vec3(1.0f));
    Texture texture("models/LocationIcon/textures/red.jpg", "albedo", 0);
    for (auto& mesh : location.meshes)
    {
        mesh.textures.clear();
        mesh.textures.push_back(texture);
    }

    // Explore screen buttons
    glm::vec3 normal = glm::vec3(1.0f);
    glm::vec3 hover = glm::vec3(0.5f);
    std::vector<Button> botonsExplore = {
        Button(glm::vec2(20, 780), glm::vec2(164, 55), nameWonders[0], normal, hover),
        Button(glm::vec2(20, 680), glm::vec2(309, 55), nameWonders[1], normal, hover),
        Button(glm::vec2(20, 580), glm::vec2(382, 55), nameWonders[2], normal, hover),
        Button(glm::vec2(20, 480), glm::vec2(427, 55), nameWonders[3], normal, hover),
        Button(glm::vec2(20, 380), glm::vec2(551, 55), nameWonders[4], normal, hover),
        Button(glm::vec2(20, 280), glm::vec2(624, 55), nameWonders[5], normal, hover),
        Button(glm::vec2(20, 180), glm::vec2(613, 55), nameWonders[6], normal, hover),
        Button(glm::vec2(20, 20), glm::vec2(200, 65), "Back", normal, hover)
    };

    // Preview screen buttons
    std::vector<Button> botonsPreview = {
        Button(glm::vec2(1484, 436), glm::vec2(200, 74), "Back", normal, hover),
        Button(glm::vec2(1429, 570), glm::vec2(310, 74), "Explore", normal, hover)
    };

    ButtonClick clickState;
    bool mousePressed = false;
    bool mouseReleased = false;

    // Orbital camera
    Camera camera(width, height, glm::vec3(0.0f));
    camera.Position = glm::vec3(0.0f, 0.0f, camera.orbitRadius);

    // Model immersion animation
    bool animationStarted = false;
    float initialAnimationTime = 0.0f;
    float lastFrame = 0.0f;

    // Earth
    float earthDuration = 2.0f;
    float initialScaleEarth = 1.0f;
    float finalScaleEarth = 3.6f;
    float earthProgress = initialScaleEarth;

    // Clouds
    float cloudDuration = 1.0f;
    float initialScaleClouds = 0.007f;
    float finalScaleClouds = 0.009f;
    float cloudsProgress = initialScaleClouds;

    // Menu status
    AppState currentState = EXPLORE_SCREEN;
    std::string title, imagePath;
    glm::vec3 cameraLoc = glm::vec3(0.0f);
    int wonder = 0, order = 0;

    while (true)
    {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Time
        float time = glfwGetTime();
        float deltaTime = time - lastFrame;
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

        // Background with texture
        glDisable(GL_DEPTH_TEST);
        imageShader.Activate();
        vaoF.Bind();
        backgroundExplore.Bind();
        backgroundExplore.texUnit(imageShader.ID, "backgroundTexture", 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glEnable(GL_DEPTH_TEST);

        // Title (except in the model immersion animation)
        if (currentState != TRAVEL_SCREEN) {
            glDisable(GL_DEPTH_TEST);
            textRenderer.RenderText("THE 7 WONDERS OF THE MODERN WEST", 110.0f, 960.0f, 1.0f, glm::vec3(rgb(103), rgb(255), rgb(45)));
            glEnable(GL_DEPTH_TEST);
        }

        if (currentState == EXPLORE_SCREEN) {
            // Computer input reading is allowed
            camera.inputsEnabled = true;

            // Button logic
            glDisable(GL_DEPTH_TEST);
            for (int i = 0; i < botonsExplore.size(); ++i) {
                bool hovered = botonsExplore[i].isHovered(mouseX, mouseY);
                bool clicked = false;
                botonsExplore[i].render(((i == 7) ? principalRenderer : buttonRenderer), hovered, false);

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

                botonsExplore[i].render(((i == 7) ? principalRenderer : buttonRenderer), hovered, (clickState.waiting == true && clickState.index == i) ? true : clicked);
            }
            glEnable(GL_DEPTH_TEST);

            if (clickState.waiting && (glfwGetTime() - clickState.clickTime) >= 0.3) {
                camera.inputsEnabled = false;

                switch (clickState.index) {
                case 0: title = nameWonders[0]; imagePath = routesWonders[0]; cameraLoc = cameraPosition[0]; order = 2; break;
                case 1: title = nameWonders[1]; imagePath = routesWonders[1]; cameraLoc = cameraPosition[1]; order = 7; break;
                case 2: title = nameWonders[2]; imagePath = routesWonders[2]; cameraLoc = cameraPosition[2]; order = 5; break;
                case 3: title = nameWonders[3]; imagePath = routesWonders[3]; cameraLoc = cameraPosition[3]; order = 4; break;
                case 4: title = nameWonders[4]; imagePath = routesWonders[4]; cameraLoc = cameraPosition[4]; order = 6; break;
                case 5: title = nameWonders[5]; imagePath = routesWonders[5]; cameraLoc = cameraPosition[5]; order = 1; break;
                case 6: title = nameWonders[6]; imagePath = routesWonders[6]; cameraLoc = cameraPosition[6]; order = 3; break;
                }
                clickState.waiting = false;

                if (clickState.index == 7) break;

                previewImage = TextureF(imagePath.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
                currentState = PREVIEW_SCREEN;
                wonder = clickState.index;
            }

            // Earth
            glm::mat4 earthTransform = glm::mat4(1.0f);
            earthTransform = glm::rotate(earthTransform, glm::radians(time * 20.0f), glm::vec3(0.0f, 0.8f, 0.1f));
            earthTransform = glm::translate(earthTransform, glm::vec3(0.0f));
            earthTransform = glm::scale(earthTransform, glm::vec3(1.0f));
            earth.DrawRotation(modelShader, camera, earthTransform);

            // Upgrade camera
            camera.OrbitInputs(window, 367.0f, glm::vec2(width / 2, height / 2));
            camera.updateMatrixExplore(45.0f, 0.1f, 100.0f);
        }
        else if (currentState == PREVIEW_SCREEN) {
            // Upgrade camera
            camera.Position = cameraLoc;
            camera.OrbitInputs(window, 367.0f, glm::vec2(width / 2, height / 2));
            camera.updateMatrixExplore(45.0f, 0.1f, 100.0f);

            // Earth
            modelShader.Activate();
            glm::mat4 earthTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
            earth.DrawRotation(modelShader, camera, earthTransform);

            // Drawing the indicator model near the Earth
            glm::mat4 locationTransform = glm::translate(glm::mat4(1.0f), indicatorPosition[wonder]);

            locationTransform = glm::rotate(locationTransform, glm::radians(indicatorRotation[wonder].x), glm::vec3(1.0f, 0.0f, 0.0f));
            locationTransform = glm::rotate(locationTransform, glm::radians(indicatorRotation[wonder].y), glm::vec3(0.0f, 1.0f, 0.0f));
            locationTransform = glm::rotate(locationTransform, glm::radians(indicatorRotation[wonder].z), glm::vec3(0.0f, 0.0f, 1.0f));

            locationTransform = glm::scale(locationTransform, glm::vec3(0.025f));
            location.DrawRotation(modelShader, camera, locationTransform);

            // Title
            float here = 1248.0f + ((672.0f - botonsExplore[wonder].size.x) / 2);
            glDisable(GL_DEPTH_TEST);
            titleRenderer.RenderText(title, here, 770.0f, 1.0f, glm::vec3(1.0f));
            glEnable(GL_DEPTH_TEST);

            // Button logic
            glDisable(GL_DEPTH_TEST);
            for (int i = 0; i < botonsPreview.size(); ++i) {
                bool hovered = botonsPreview[i].isHovered(mouseX, mouseY);
                bool clicked = false;
                botonsPreview[i].render(principalRenderer, hovered, false);

                if (hovered && mousePressed && !clickState.waiting) {
                    clicked = true;
                    audio.playClickSound((i == 0) ? "media/click.mp3" : "media/clickTravel.mp3");
                    clickState.index = i;
                    clickState.clickTime = glfwGetTime();
                    clickState.waiting = true;
                }

                botonsPreview[i].render(principalRenderer, hovered, (clickState.waiting == true && clickState.index == i) ? true : clicked);
            }
            glEnable(GL_DEPTH_TEST);

            if (clickState.waiting && (glfwGetTime() - clickState.clickTime) >= 0.3) {
                switch (clickState.index) {
                case 0: currentState = EXPLORE_SCREEN; break;
                case 1: currentState = TRAVEL_SCREEN; break;
                }
                camera = Camera(width, height, glm::vec3(0.0f));
                camera.Position = glm::vec3(0.0f, 0.0f, camera.orbitRadius);
                clickState.waiting = false;
                camera.inputsEnabled = false;
            }

            // Draw background panel behind image
            glDisable(GL_DEPTH_TEST);
            panelShader.Activate();
            panelVAO.Bind();
            glUniform4f(glGetUniformLocation(panelShader.ID, "panelColor"), 0.0f, 0.0f, 0.0f, 0.55f);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glEnable(GL_DEPTH_TEST);

            // Draw wonder image
            glDisable(GL_DEPTH_TEST);
            imageShader.Activate();
            imagevaoF.Bind();
            previewImage.Bind();
            previewImage.texUnit(imageShader.ID, "backgroundTexture", 0);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glEnable(GL_DEPTH_TEST);
        }
        else if (currentState == TRAVEL_SCREEN) {

            // Animation logic
            if (!animationStarted) {
                animationStarted = true;
                initialAnimationTime = glfwGetTime();
                earthProgress = initialScaleEarth;
                cloudsProgress = initialScaleClouds;
            }

            // Upgrade camera
            camera.Position = cameraLoc;
            camera.OrbitInputs(window, 367.0f, glm::vec2(width / 2, height / 2));
            camera.updateMatrixExplore(45.0f, 0.1f, 100.0f);

            float currentTime = glfwGetTime() - initialAnimationTime;

            // Earth animation
            if (currentTime <= earthDuration) {
                modelShader.Activate();
                earthProgress = initialScaleEarth + (finalScaleEarth - initialScaleEarth) * (currentTime / earthDuration);
                glm::mat4 earthTransform = glm::mat4(1.0f);
                earthTransform = glm::scale(earthTransform, glm::vec3(earthProgress));
                earth.DrawRotation(modelShader, camera, earthTransform);
            }

            // Clouds animation
            if (currentTime > earthDuration && currentTime < (earthDuration + cloudDuration)) {
                modelShader.Activate();
                float tiempoNubes = currentTime - earthDuration;
                cloudsProgress = initialScaleClouds + (finalScaleClouds - initialScaleClouds) * (tiempoNubes / cloudDuration);
                glm::mat4 animationTransform = glm::mat4(1.0f);
                animationTransform = glm::scale(animationTransform, glm::vec3(cloudsProgress));
                animation.DrawRotation(modelShader, camera, animationTransform);
            }

            // End animation after seconds
            if (currentTime >= earthDuration + cloudDuration) {
                audio.stopBackgroundMusic();
                LoadingScreen(window, modelShader, glassModel, 5.0f);

                // Next part of the program (Exploration of the Wonder)
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
                SceneModels(order, window, model);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                // Return to the initial state
                animationStarted = false;
                currentState = EXPLORE_SCREEN;
                camera = Camera(width, height, glm::vec3(0.0f));
                camera.Position = glm::vec3(0.0f, 0.0f, camera.orbitRadius);
                audio.playBackgroundMusic("media/outerSpace.mp3");
            }
        }

        glEnable(GL_DEPTH_TEST);
        glfwSwapBuffers(window);
    }

    audio.stopBackgroundMusic();
}