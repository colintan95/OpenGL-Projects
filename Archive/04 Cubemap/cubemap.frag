#version 330 core

in vec3 fTexCoord;

out vec4 outColor;

uniform samplerCube uCubemap;

void main() {
    outColor = texture(uCubemap, fTexCoord);
}