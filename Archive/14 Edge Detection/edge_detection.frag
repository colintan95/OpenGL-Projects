#version 330 core

in vec2 vfTexCoord;

out vec4 fColor;

uniform sampler2D uTexture;

uniform int uScreenWidth;
uniform int uScreenHeight;

uniform float uEdgeThreshold;

float luma(vec3 color) {
    return (0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b);
}

void main() {
    float dx = 1.0 / float(uScreenWidth);
    float dy = 1.0 / float(uScreenHeight);
    
    float s00 = luma(texture(uTexture, vfTexCoord + vec2(-dx, dy)).rgb);
    float s01 = luma(texture(uTexture, vfTexCoord + vec2(0, dy)).rgb);
    float s02 = luma(texture(uTexture, vfTexCoord + vec2(dx, dy)).rgb);
    float s10 = luma(texture(uTexture, vfTexCoord + vec2(-dx, 0)).rgb);
    float s11 = luma(texture(uTexture, vfTexCoord + vec2(0, 0)).rgb);
    float s12 = luma(texture(uTexture, vfTexCoord + vec2(dx, 0)).rgb);
    float s20 = luma(texture(uTexture, vfTexCoord + vec2(-dx, -dy)).rgb);
    float s21 = luma(texture(uTexture, vfTexCoord + vec2(0, -dy)).rgb);
    float s22 = luma(texture(uTexture, vfTexCoord + vec2(dx, -dy)).rgb);
    
    float sx = s02 + 2 * s12 + s22 - (s00 + 2 * s10 + s20);
    float sy = s20 + 2 * s21 + s22 - (s00 + 2 * s01 + s02);
    
    float dist = sx * sx + sy * sy;
    
    if (dist > uEdgeThreshold) {
        fColor = vec4(1.0);
    }
    else {
        fColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}