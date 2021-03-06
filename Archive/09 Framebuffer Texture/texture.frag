#version 330 core

in vec2 fTexCoord;

out vec4 outColor;

uniform sampler2D uTexture;

void main() {
    outColor = texture(uTexture, fTexCoord);
}