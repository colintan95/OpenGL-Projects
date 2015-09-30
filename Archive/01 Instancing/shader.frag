#version 330

in vec3 FragColor;
out vec4 color;

in vec4 position;

void main() {
    color = vec4(FragColor, 0.9);
}