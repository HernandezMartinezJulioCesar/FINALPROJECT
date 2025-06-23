#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture_Scene.h"
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Camera.h"

// Variables globales para recursos compartidos
Shader* wallShader = nullptr;
VAO* wallVAO = nullptr;
VBO* wallVBO = nullptr;
EBO* wallEBO = nullptr;
Texture_Scene* wallTexture = nullptr;
GLfloat* currentVertices = nullptr;
size_t currentVerticesSize = 0;

Texture_Scene* loadTexture(const char* filename) {
    // Esto evita recargar la misma textura
    static std::unordered_map<std::string, Texture_Scene*> textureCache;

    if (textureCache.find(filename) != textureCache.end()) {
        return textureCache[filename];
    }

    Texture_Scene* tex = new Texture_Scene(filename, GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
    textureCache[filename] = tex;
    return tex;
}

void initializeWallResources(const char* filename) {
    // Inicializar recursos compartidos una sola vez
    if (!wallShader) {
        wallShader = new Shader("default_scenes.vert", "default_scenes.frag");

        // Configuración inicial de iluminación
        glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        glm::vec3 lightPos = glm::vec3(1.0f, 1.0f, 1.0f);

        wallShader->Activate();
        glUniform4f(glGetUniformLocation(wallShader->ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
        glUniform3f(glGetUniformLocation(wallShader->ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);


        // Configurar textura
        wallTexture = new Texture_Scene(filename, GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
        wallTexture->texUnit(*wallShader, "tex0", 0);

        // Configurar VAO, VBO y EBO genéricos
        wallVAO = new VAO();
        wallVAO->Bind();

        // Crear un VBO inicial con tamaño suficiente (por ejemplo, para 4 vértices)
        currentVerticesSize = 4 * 11 * sizeof(GLfloat); // 4 vértices * 11 floats cada uno
        currentVertices = new GLfloat[4 * 11]; // Espacio para 4 vértices
        wallVBO = new VBO(currentVertices, currentVerticesSize);

        // Los índices son siempre los mismos para cuadrados/rectángulos
        GLuint indices[] = { 0, 1, 2, 0, 2, 3 };
        wallEBO = new EBO(indices, sizeof(indices));

        // Configurar atributos (asumiendo el mismo formato de vértices)
        wallVAO->LinkAttrib(*wallVBO, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
        wallVAO->LinkAttrib(*wallVBO, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
        wallVAO->LinkAttrib(*wallVBO, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
        wallVAO->LinkAttrib(*wallVBO, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));

        wallVAO->Unbind();
        wallVBO->Unbind();
        wallEBO->Unbind();
    }
    wallTexture = loadTexture(filename);
    wallTexture->texUnit(*wallShader, "tex0", 0);
}

void cleanupWallResources() {
    if (wallShader) delete wallShader;
    if (wallVAO) delete wallVAO;
    if (wallVBO) delete wallVBO;
    if (wallEBO) delete wallEBO;
    if (wallTexture) delete wallTexture;
    if (currentVertices) delete[] currentVertices;

    static std::unordered_map<std::string, Texture_Scene*> textureCache;
    for (auto& pair : textureCache) {
        delete pair.second;
    }
    textureCache.clear();
}

void renderParedes(GLfloat* vertices, int numVertices, GLFWwindow* window, Camera& camera, const char* filename) {
    // Inicializar recursos si no lo están
    initializeWallResources(filename);

    // Verificar si necesitamos redimensionar nuestro buffer
    size_t requiredSize = numVertices * sizeof(GLfloat);
    if (requiredSize > currentVerticesSize) {
        // Liberar el antiguo VBO y crear uno nuevo con más capacidad
        delete wallVBO;
        delete[] currentVertices;

        currentVerticesSize = requiredSize * 2; // Asignar el doble para evitar realloc frecuentes
        currentVertices = new GLfloat[numVertices];
        wallVBO = new VBO(currentVertices, currentVerticesSize);

        // Reconfigurar los atributos del VAO
        wallVAO->Bind();
        wallVAO->LinkAttrib(*wallVBO, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
        wallVAO->LinkAttrib(*wallVBO, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
        wallVAO->LinkAttrib(*wallVBO, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
        wallVAO->LinkAttrib(*wallVBO, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));
        wallVAO->Unbind();
    }

    // Copiar los nuevos vértices al buffer
    memcpy(currentVertices, vertices, numVertices * sizeof(GLfloat));

    // Actualizar los datos del VBO
    wallVBO->Bind();
    glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * sizeof(GLfloat), currentVertices);
    wallVBO->Unbind();

    // Configurar transformaciones y renderizar
    glm::vec3 objectPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 objectModel = glm::mat4(1.0f);
    objectModel = glm::translate(objectModel, objectPos);

    wallShader->Activate();
    glUniformMatrix4fv(glGetUniformLocation(wallShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(objectModel));
    glUniform3f(glGetUniformLocation(wallShader->ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);

    camera.Matrix(*wallShader, "camMatrix");
    wallTexture->Bind();

    wallVAO->Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // 6 índices para 2 triángulos
    wallVAO->Unbind();
}
/*void calcularResolucionOptimaTextura(GLfloat* vertices, int numVertices) {
    // Asumimos que cada vértice tiene 11 componentes (3 posición, 3 color, 2 texCoord, 3 normal)
    const int componentesPorVertice = 11;
    const int offsetTexCoord = 6; // Las coordenadas de textura están en las posiciones 6 y 7

    // 1. Calcular el tamaño físico de la pared en unidades del mundo
    float minX = vertices[0], maxX = vertices[0];
    float minY = vertices[1], maxY = vertices[1];
    float minZ = vertices[2], maxZ = vertices[2];

    for (int i = 0; i < numVertices; ++i) {
        int base = i * componentesPorVertice;
        minX = std::min(minX, vertices[base]);
        maxX = std::max(maxX, vertices[base]);
        minY = std::min(minY, vertices[base + 1]);
        maxY = std::max(maxY, vertices[base + 1]);
        minZ = std::min(minZ, vertices[base + 2]);
        maxZ = std::max(maxZ, vertices[base + 2]);
    }

    float ancho = maxX - minX;
    float alto = maxY - minY;
    float profundidad = maxZ - minZ;

    // Determinar las dimensiones principales de la pared
    float areaFisica;
    if (alto > ancho && alto > profundidad) {
        // Pared vertical (ancho x alto)
        areaFisica = ancho * alto;
    }
    else {
        // Pared horizontal (ancho x profundidad)
        areaFisica = ancho * profundidad;
    }

    // 2. Calcular el área de textura utilizada (coordenadas UV)
    float minU = vertices[offsetTexCoord], maxU = vertices[offsetTexCoord];
    float minV = vertices[offsetTexCoord + 1], maxV = vertices[offsetTexCoord + 1];

    for (int i = 0; i < numVertices; ++i) {
        int base = i * componentesPorVertice + offsetTexCoord;
        minU = std::min(minU, vertices[base]);
        maxU = std::max(maxU, vertices[base]);
        minV = std::min(minV, vertices[base + 1]);
        maxV = std::max(maxV, vertices[base + 1]);
    }

    float anchoUV = maxU - minU;
    float altoUV = maxV - minV;
    float areaUV = anchoUV * altoUV;

    // 3. Calcular resolución óptima (asumiendo 100 píxeles por unidad de mundo para calidad HD)
    const float pixelesPorUnidad = 100.0f;
    float resolucionOptima = sqrt(areaFisica * pixelesPorUnidad * pixelesPorUnidad / areaUV);

    // Redondear a valores típicos de resolución
    int resolucionRedondeada = static_cast<int>(resolucionOptima);

    // Ajustar a resoluciones estándar
    const int resoluciones[] = { 128, 256, 512, 1024, 2048, 4096 };
    int mejorResolucion = resoluciones[0];

    for (int res : resoluciones) {
        if (abs(res - resolucionRedondeada) < abs(mejorResolucion - resolucionRedondeada)) {
            mejorResolucion = res;
        }
    }

    // Mostrar resultados en consola
    std::cout << "\n=== CALCULO DE RESOLUCION OPTIMA ===" << std::endl;
    std::cout << "Dimensiones fisicas: " << ancho << " x " << (alto > profundidad ? alto : profundidad) << " unidades" << std::endl;
    std::cout << "Area UV utilizada: " << anchoUV << " x " << altoUV << std::endl;
    std::cout << "Resolucion calculada: " << resolucionRedondeada << " px" << std::endl;
    std::cout << "Resolucion recomendada: " << mejorResolucion << " x " << mejorResolucion << " px" << std::endl;
    std::cout << "===================================\n" << std::endl;
}*/