#version 330 core

out vec4 FragColor;

in vec3 crntPos;
in vec3 Normal;
in vec3 color;
in vec2 texCoord;

// Texturas para cada cara del cubo
uniform sampler2D texture0; // Cielo (superior)
uniform sampler2D texture1; // Frontal
uniform sampler2D texture2; // Trasera
uniform sampler2D texture3; // Izquierda
uniform sampler2D texture4; // Derecha

// Texturas originales (difusa y especular)
uniform sampler2D diffuse0;
uniform sampler2D specular0;

uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

uniform float flashlightIntensity;
uniform vec3 flashlightColor;

// Nueva variable para indicar si usamos texturas del cubo
uniform int useCubeTextures;

// Variables para la linterna
uniform bool flashlightOn;
uniform vec3 flashlightPos;
uniform vec3 flashlightDir;
uniform float flashlightCutOff;
uniform float flashlightOuterCutOff;

vec4 getBaseTexture() {
    if (useCubeTextures == 1) {
        // Determinar qu� textura usar basada en la normal
        vec3 norm = normalize(Normal);
        
        if (abs(norm.y) > 0.9) {
            // Cara superior/inferior
            return texture(texture0, texCoord);
        } else if (abs(norm.z) > 0.9) {
            // Cara frontal/trasera
            if (norm.z > 0.0) 
                return texture(texture1, texCoord);
            else 
                return texture(texture2, texCoord);
        } else {
            // Caras laterales
            if (norm.x > 0.0) 
                return texture(texture4, texCoord);
            else 
                return texture(texture3, texCoord);
        }
    } else {
        // Usar la textura original
        return texture(diffuse0, texCoord);
    }
}

vec4 pointLight() {	
	vec3 lightVec = lightPos - crntPos;
	float dist = length(lightVec);
	float a = 3.0;
	float b = 0.7;
	float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

	float ambient = 0.20f;
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (getBaseTexture() * (diffuse * inten + ambient) + texture(specular0, texCoord).r * specular * inten) * lightColor;
}

vec4 direcLight() {
    // Aumentamos la luz ambiental para que el modelo nunca quede oscuro
    float ambient = 0.77f;  // Intensidad m�xima (antes era 0.4f)
    
    // Direcci�n de la luz (puedes ajustarla seg�n necesites)
    vec3 lightDirection = normalize(vec3(0.77f, 0.33f, 0.22f));
    
    // Componente difusa (suave para no saturar)
    vec3 normal = normalize(Normal);
    float diffuse = max(dot(normal, lightDirection), 0.0f) * 0.5f; // Reducimos difusa
    
    // Componente especular (opcional, para darle un poco de brillo)
    float specularLight = 0.2f;  // Reducimos el brillo para no exagerar
    vec3 viewDirection = normalize(camPos - crntPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
    float specular = specAmount * specularLight;
    
    // Resultado final (asegurando que la luz ambiental sea fuerte)
    vec4 baseColor = getBaseTexture();
    return baseColor * (diffuse + ambient) + texture(specular0, texCoord).r * specular;
}
vec4 flashlightEffectinitial() {
    // --- Configuraci�n fija (radio grande/intensidad alta) ---
    float outerCutOff = cos(radians(70.0f));  // �ngulo muy amplio (70 grados)
    float cutOff = cos(radians(70.0f));       // �ngulo a�n m�s amplio
    float intensity = 1.8;                    // Intensidad alta (para iluminar todo)

    vec3 lightDir = normalize(vec3(-1.0, -1.0, -1.0)); // Direcci�n fija (hacia adelante/abajo)
    
    // --- C�lculo de la luz ---
    vec3 lightToFragment = normalize(crntPos - flashlightPos);
    float theta = dot(lightToFragment, lightDir);
    float epsilon = cutOff - outerCutOff;
    float attenuation = 1.0 / (1.2 + 0.002 * length(flashlightPos - crntPos)); // Atenuaci�n m�nima

    // --- Iluminaci�n difusa + especular (sin condiciones de corte) ---
    vec3 normal = normalize(Normal);
    float diffuse = max(dot(normal, -lightDir), 0.0);
    vec3 viewDir = normalize(camPos - crntPos);
    vec3 reflectDir = reflect(lightToFragment, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    // --- Resultado final (siempre aplicado, sin "if") ---
    vec4 baseColor = getBaseTexture();
    vec3 result = baseColor.rgb * diffuse * intensity * attenuation;
    return vec4(result, baseColor.a);
}
vec4 flashlightEffect() {
    //float flashlightStrength = 4.0;
    vec3 lightToFragment = normalize(crntPos - flashlightPos);
    vec3 normal = normalize(Normal);
    
    // C�lculo del �ngulo
    float theta = dot(lightToFragment, normalize(flashlightDir));
    float epsilon = flashlightCutOff - flashlightOuterCutOff;
    float intensity = clamp((theta - flashlightOuterCutOff) / epsilon, 0.0, 1.0);
    
    // Solo calculamos si est� dentro del cono
    if (theta > flashlightOuterCutOff) {
        // Atenuaci�n
        float distance = length(flashlightPos - crntPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
        
        // Componente difusa
        float diffuse = max(dot(normal, -lightToFragment), 0.0);
        
        // Componente especular
        vec3 viewDir = normalize(camPos - crntPos);
        vec3 reflectDir = reflect(lightToFragment, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        
        // Color base con la textura
        vec4 baseColor = getBaseTexture();
        
        // Combinamos todo
       vec3 result = (baseColor.rgb * diffuse + texture(specular0, texCoord).r * spec) * intensity * attenuation * flashlightColor * flashlightIntensity;
        return vec4(result, baseColor.a);
    }
    return vec4(0.0);
}
void main() {
    vec4 result = flashlightEffectinitial();
    
    if (flashlightOn) {
        result += flashlightEffect();
    }
    
    FragColor = result;
}