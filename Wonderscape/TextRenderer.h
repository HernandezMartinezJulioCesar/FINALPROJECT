#pragma once
#include <map>
#include <string>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "shaderClassF.h"

struct Character {
    unsigned int TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    unsigned int Advance;
};

class TextRenderer {
public:
    std::map<char, Character> Characters;
    unsigned int VAO, VBO;
    ShaderF shader;

    TextRenderer(unsigned int width, unsigned int height);
    void Load(std::string fontPath, unsigned int fontSize);
    void RenderText(std::wstring text, float x, float y, float scale, glm::vec3 color);

};
