#include "TextureF.h"

TextureF::TextureF(const char* imagePath, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
    type = texType;

    int widthImg, heightImg, numColCh;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* bytes = stbi_load(imagePath, &widthImg, &heightImg, &numColCh, 0);

    glGenTextures(1, &ID);
    glActiveTexture(slot);
    glBindTexture(type, ID);

    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(type, 0, format, widthImg, heightImg, 0, format, pixelType, bytes);
    glGenerateMipmap(type);

    stbi_image_free(bytes);
    glBindTexture(type, 0);
}

void TextureF::texUnit(GLuint shaderID, const char* uniform, GLuint unit)
{
    GLuint texUni = glGetUniformLocation(shaderID, uniform);
    glUseProgram(shaderID);
    glUniform1i(texUni, unit);
}

void TextureF::Bind()
{
    glBindTexture(type, ID);
}

void TextureF::Unbind()
{
    glBindTexture(type, 0);
}

void TextureF::Delete()
{
    glDeleteTextures(1, &ID);
}
