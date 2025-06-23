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

    Button(const glm::vec2& pos, const glm::vec2& size, const std::string& label);

    bool isHovered(double mouseX, double mouseY) const;
    void render(TextRenderer& textRenderer, bool hovered, bool clicked) const;
};

#endif
