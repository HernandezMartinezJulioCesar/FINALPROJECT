// Configuración de ventana
constexpr unsigned int width = 1920;
constexpr unsigned int height = 1080;

// Estados de la aplicación
enum AppState {
    MENU_PRINCIPAL,
    PANTALLA_EXPLORAR,
    PANTALLA_CREDITOS,
    PANTALLA_AYUDA,
    PANTALLA_SALIR,
    PANTALLA_MARAVILLA,
    PANTALLA_VISTA_PREVIA
};

// Estructura para manejo de clicks
struct ButtonClick {
    int index = -1;
    double clickTime = 0.0;
    bool waiting = false;
};

