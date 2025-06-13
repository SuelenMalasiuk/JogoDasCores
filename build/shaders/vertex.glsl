#version 330 core

layout (location = 0) in vec2 aPos;

uniform vec3 uColor;
uniform vec2 uOffset;
uniform vec2 uScale;

out vec3 color;

void main() {
    vec2 scaledPos = aPos * uScale;  // escala largura e altura
    vec2 finalPos = scaledPos + uOffset;  // move para posição correta
    gl_Position = vec4(finalPos, 0.0, 1.0);
    color = uColor;
}