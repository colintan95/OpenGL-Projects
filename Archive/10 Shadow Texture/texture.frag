#version 330 core

in vec2 fTexCoord;

out vec4 outColor;

uniform sampler2D uTexture;

void main() {
    float f = 1000;
    float n = 0.1;
    
    float depth = texture(uTexture, fTexCoord).r;
    
    float z = (2 * n) / (f + n - depth * (f-n)) * 0.5 + 0.5;
    
    outColor = vec4(z, z, z, 1);
}