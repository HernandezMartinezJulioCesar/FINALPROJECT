// Configuraci�n de ventana
constexpr unsigned int width = 1920;
constexpr unsigned int height = 1080;

// Estados de la aplicaci�n
enum AppState {
    MENU_PRINCIPAL,
    PANTALLA_EXPLORAR,
    PANTALLA_CREDITOS,
    PANTALLA_AYUDA,
    PANTALLA_SALIR,
    PANTALLA_VISTA_PREVIA,
    PANTALLA_MARAVILLA,
    PANTALLA_VIAJAR
};

// Estructura para manejo de clicks
struct ButtonClick {
    int index = -1;
    double clickTime = 0.0;
    bool waiting = false;
};

struct GLFWwindow;
class Shader;
class Model;
void PantallaCarga(GLFWwindow* window, Shader& modelShader, Model& lupaModel, float duracion = 3.0f);
