#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>

#include "Settings.h"
#include "MainMenu.h"
#include "WondersMenu.h"

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

int MainMenu() {
    // Initialization of the window
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

    // Load window icon
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

    // Load audio
    AudioManager audio;
    audio.playBackgroundMusic("media/environment.mp3");
    audio.setMusicVolume(1.0f);
    audio.setEffectsVolume(0.4f);

    // Load text
    TextRenderer textRenderer(width, height), buttonRenderer(width, height), creditsRenderer(width, height);
    textRenderer.Load("fonts/River Adventurer.ttf", 200);
    buttonRenderer.Load("fonts/JungleAdventurer.ttf", 100);
    creditsRenderer.Load("fonts/Cute Dino.ttf", 80);

    // Load backgrounds
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

    // Base for the model of the Earth
    ShaderF circleShader("circle.vert", "circle.frag");
    Circle circle(1.38f, 0.0f, 0.80f);

    // Load models to use in the program
    Shader modelShader("default.vert", "defaultModel.frag");
    glm::vec4 lightColor = glm::vec4(1.0f);

    modelShader.Activate();
    glUniform4f(glGetUniformLocation(modelShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);

    Camera camera(width, height, glm::vec3(0.0f));
    Model earth("models/Earth/scene.gltf", glm::vec3(1.0f));
    Model clouds("models/Clouds/scene.gltf", glm::vec3(1.0f));
    Model uni("models/LogoModel/UNI.gltf", glm::vec3(1.0f));
    Model glass("models/MagnifyingGlass/scene.gltf", glm::vec3(1.0f));
    Model animation("models/LoadingScreen/scene.gltf", glm::vec3(1.0f));
    Model model("models/Wonders/scene.gltf", glm::vec3(0.45f));

    // Manually load textures for the Logo and the Magnifying Glass
    Texture texture("models/LogoModel/textures/Image_0.jpg", "albedo", 0);
    for (auto& mesh : uni.meshes) {
        mesh.textures.clear();
        mesh.textures.push_back(texture);
    }
    texture = Texture("models/MagnifyingGlass/texture/Image_0.jpg", "albedo", 0);
    for (auto& mesh : glass.meshes) {
        mesh.textures.clear();
        mesh.textures.push_back(texture);
    }

    // Buttons
    std::vector<Button> botonsMenu = {
        Button(glm::vec2(170, 500), glm::vec2(310, 74), "Explore"),
        Button(glm::vec2(170, 400), glm::vec2(180, 74), "Help"),
        Button(glm::vec2(170, 300), glm::vec2(295, 74), "Credits"),
        Button(glm::vec2(170, 200), glm::vec2(160, 74), "Exit")
    };

    Button backButton = Button(glm::vec2(20, 1000), glm::vec2(200, 75), "Back");

    ButtonClick clickState;
    bool mousePressed = false;
    bool mouseReleased = false;

    // Logic of the clouds
    srand(static_cast<unsigned int>(time(0)));

    std::vector<glm::vec3> fixedPositions = {
        glm::vec3(1.6f,  -0.8f, 4.0f),  // top left corner
        glm::vec3(-1.6f,  -0.8f, 4.0f), // top right corner
        glm::vec3(1.6f, 0.8f, 4.0f),    // bottom left corner
        glm::vec3(-1.6f, 0.8f, 4.0f),   // bottom right corner
        glm::vec3(0.0f,  0.1f, 4.0f),   // center
        glm::vec3(1.6f,  0.1f, 4.0f),   // centro left
        glm::vec3(0.0f,  -0.8f, 4.0f),  // top center
        glm::vec3(0.0f, 0.8f, 4.0f)     // bottom center
    };

    // Initial position and target of the cloud
    glm::vec3 cloudPosition = fixedPositions[rand() % fixedPositions.size()];
    glm::vec3 cloudEndPosition = fixedPositions[rand() % fixedPositions.size()];
    float cloudSpeed = 0.05f;

    // Menu status
    AppState currentStatus = MAIN_MENU;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Camera and models
        modelShader.Activate();
        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        // Time
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

        if (currentStatus == MAIN_MENU) {

            // Background with texture
            glDisable(GL_DEPTH_TEST);
            backgroundShader.Activate();
            vaoF.Bind();
            backgroundMenu.Bind();
            backgroundMenu.texUnit(backgroundShader.ID, "backgroundTexture", 0);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glEnable(GL_DEPTH_TEST);

            // Cloud animation
            glm::vec3 direction = cloudEndPosition - cloudPosition;
            float distance = glm::length(direction);

            if (distance < 0.1f)
                cloudEndPosition = fixedPositions[rand() % fixedPositions.size()];

            if (distance > 0.0f)
                cloudPosition += glm::normalize(direction) * cloudSpeed * deltaTime;

            glm::mat4 cloudTransform = glm::translate(glm::mat4(1.0f), cloudPosition);
            cloudTransform = glm::scale(cloudTransform, glm::vec3(0.5f));

            clouds.DrawRotation(modelShader, camera, cloudTransform);

            // Base and Earth
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

            // Title
            glDisable(GL_DEPTH_TEST);
            textRenderer.RenderText("Wonder", 138.0f, height - 219.0f, 1.04f, glm::vec3(0.2f));
            textRenderer.RenderText("Scapes", 138.0f, height - 399.0f, 1.04f, glm::vec3(0.2f));
            textRenderer.RenderText("Wonder", 150.0f, height - 220.0f, 1.0f, glm::vec3(rgb(230), rgb(57), rgb(70)));
            textRenderer.RenderText("Scapes", 150.0f, height - 400.0f, 1.0f, glm::vec3(rgb(244), rgb(162), rgb(97)));
            glEnable(GL_DEPTH_TEST);

            // Button logic
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
                case 0: currentStatus = EXPLORE_SCREEN; break;
                case 1: currentStatus = HELP_SCREEN; break;
                case 2: currentStatus = CREDITS_SCREEN; break;
                case 3: currentStatus = EXIT; break;
                }
                clickState.waiting = false;
            }
        }
        else if (currentStatus == HELP_SCREEN || currentStatus == CREDITS_SCREEN) {

            if (currentStatus == HELP_SCREEN) {
                // Background with texture
                glDisable(GL_DEPTH_TEST);
                backgroundShader.Activate();
                vaoF.Bind();
                backgroundHelp.Bind();
                backgroundHelp.texUnit(backgroundShader.ID, "backgroundTexture", 0);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glEnable(GL_DEPTH_TEST);
            }
            else if (currentStatus == CREDITS_SCREEN) {
                // Background with texture
                glDisable(GL_DEPTH_TEST);
                backgroundShader.Activate();
                vaoF.Bind();
                backgroundCredits.Bind();
                backgroundCredits.texUnit(backgroundShader.ID, "backgroundTexture", 0);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glEnable(GL_DEPTH_TEST);

                // Logo model
                float angleDegrees = sin(time * 1.5f) * 25.0f;
                glm::mat4 uniTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, 0.3f, 5.0f));
                uniTransform = glm::rotate(uniTransform, glm::radians(angleDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
                uniTransform = glm::scale(uniTransform, glm::vec3(0.5f));
                uni.DrawRotation(modelShader, camera, uniTransform);
            }

            // Button logic
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
                currentStatus = MAIN_MENU;
                clickState.waiting = false;
            }
        }
        else if (currentStatus == EXPLORE_SCREEN) {
            audio.stopBackgroundMusic();

            // Next part of the program (Wonder Choice)
            WondersMenu(window, modelShader, earth, glass, animation, model);

            currentStatus = MAIN_MENU;
            audio.playBackgroundMusic("media/environment.mp3");
        }
        else if (currentStatus == EXIT) {
            break;
        }

        glEnable(GL_DEPTH_TEST);
        glfwSwapBuffers(window);
    }

    audio.stopBackgroundMusic();

    // Disposal of used objects
    circle.Delete();
    vaoF.Delete(); vboF.Delete(); eboF.Delete();
    circleShader.Delete(); backgroundShader.Delete(); modelShader.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}