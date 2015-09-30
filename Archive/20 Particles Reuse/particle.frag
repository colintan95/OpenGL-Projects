#version 330 core

in float vfAlpha;

out vec4 fColor;

uniform sampler2D uTexture;

void main() {
    fColor = texture(uTexture, gl_PointCoord);
    fColor.a *= vfAlpha;
}