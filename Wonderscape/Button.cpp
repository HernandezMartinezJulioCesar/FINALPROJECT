#include "Button.h"

float rg(int color) {
    return color / 255.0f;
}

Button::Button(const glm::vec2& pos, const glm::vec2& size, const std::string& label)
    : position(pos), size(size), label(label) {
}

bool Button::isHovered(double mouseX, double mouseY) const {
    return mouseX >= position.x && mouseX <= position.x + size.x &&
        mouseY >= position.y && mouseY <= position.y + size.y;
}

void Button::render(TextRenderer& textRenderer, float screenHeight, bool hovered, bool clicked) const {
    float textX = position.x;
    float textY = position.y;

    glm::vec3 color;

    if (clicked)
        color = glm::vec3(rg(204), rg(51), rg(51));
    else if (hovered)
        color = glm::vec3(rg(255), rg(255), rg(255));
    else
        color = glm::vec3(rg(0), rg(0), rg(0));

    std::wstring wideLabel(label.begin(), label.end());
    textRenderer.RenderText(wideLabel, textX, textY, 1.0f, color);
}
