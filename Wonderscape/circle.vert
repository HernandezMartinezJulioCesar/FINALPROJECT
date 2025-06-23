#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 projection;
uniform vec2 scale;

void main()
{
    vec2 scaledPos = aPos * scale;
    gl_Position = projection * vec4(scaledPos, 0.0, 1.0);
}
