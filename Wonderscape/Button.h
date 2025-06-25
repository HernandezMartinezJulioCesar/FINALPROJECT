#ifndef BUTTON_H
#define BUTTON_H

#include <glm/glm.hpp>
#include <string>
#include "TextRenderer.h"

class Button {
public:
    glm::vec2 position;
    glm::vec2 size;
    std::string label;
    glm::vec3 normalColor;
    glm::vec3 hoverColor;
    glm::vec3 clickColor;

    Button(const glm::vec2& pos, const glm::vec2& size, const std::string& label,
        const glm::vec3& normalColor = glm::vec3(0.05f),
        const glm::vec3& hoverColor = glm::vec3(1.0f),
        const glm::vec3& clickColor = glm::vec3(0.8f, 0.2f, 0.2f));

    bool isHovered(double mouseX, double mouseY) const;
    void render(TextRenderer& textRenderer, bool hovered, bool clicked) const;
};

#endif
