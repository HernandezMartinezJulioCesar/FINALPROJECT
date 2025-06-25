#include "Button.h"

Button::Button(const glm::vec2& pos, const glm::vec2& size, const std::string& label, const glm::vec3& normalColor, const glm::vec3& hoverColor, const glm::vec3& clickColor)
    : position(pos), size(size), label(label), normalColor(normalColor), hoverColor(hoverColor), clickColor(clickColor) {
}

bool Button::isHovered(double mouseX, double mouseY) const {
    return mouseX >= position.x && mouseX <= position.x + size.x &&
        mouseY >= position.y && mouseY <= position.y + size.y;
}

void Button::render(TextRenderer& textRenderer, bool hovered, bool clicked) const {
    float textX = position.x;
    float textY = position.y;

    glm::vec3 color = normalColor;

    if (clicked)
        color = clickColor;
    else if (hovered)
        color = hoverColor;

    textRenderer.RenderText(label, textX, textY, 1.0f, color);
}
