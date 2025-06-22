#include "Button.h"

Button::Button(const glm::vec2& pos, const glm::vec2& size, const std::string& label)
    : position(pos), size(size), label(label) {
}

bool Button::isHovered(double mouseX, double mouseY) const {
    return mouseX >= position.x && mouseX <= position.x + size.x &&
        mouseY >= position.y && mouseY <= position.y + size.y;
}

void Button::render(TextRenderer& textRenderer, bool hovered, bool clicked) const {
    float textX = position.x;
    float textY = position.y;

    glm::vec3 color;

    if (clicked)
        color = glm::vec3(0.8f, 0.2f, 0.2f);
    else if (hovered)
        color = glm::vec3(1.0f);
    else
        color = glm::vec3(0.0f);

    std::wstring wideLabel(label.begin(), label.end());
    textRenderer.RenderText(wideLabel, textX, textY, 1.0f, color);
}
