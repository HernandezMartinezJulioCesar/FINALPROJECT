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
        // Determinar qué textura usar basada en la normal
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
	float ambient = 0.21f;
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 1.0f));
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	float specularLight = 0.55f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (getBaseTexture() * diffuse + getBaseTexture() * ambient + texture(specular0, texCoord).r * specular) * lightColor;
}

vec4 flashlightEffect() {
    //float flashlightStrength = 4.0;
    vec3 lightToFragment = normalize(crntPos - flashlightPos);
    vec3 normal = normalize(Normal);
    
    // Cálculo del ángulo
    float theta = dot(lightToFragment, normalize(flashlightDir));
    float epsilon = flashlightCutOff - flashlightOuterCutOff;
    float intensity = clamp((theta - flashlightOuterCutOff) / epsilon, 0.0, 1.0);
    
    // Solo calculamos si está dentro del cono
    if (theta > flashlightOuterCutOff) {
        // Atenuación
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
    vec4 result = direcLight();
    
    if (flashlightOn) {
        result += flashlightEffect();
    }
    
    FragColor = result;
}