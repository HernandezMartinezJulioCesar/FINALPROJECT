#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;



// Gets the Texture Units from the main function
uniform sampler2D diffuse0;
// Gets the color of the light from the main function
uniform vec4 lightColor;


vec4 direcLight()
{
    float ambient = 0.9f;
    vec4 texColor = texture(diffuse0, texCoord);
    return vec4(texColor.rgb * ambient * lightColor.rgb, 1.0); // <- Forzar opacidad
}


void main ()
{
    FragColor = direcLight();
}