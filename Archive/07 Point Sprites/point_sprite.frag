#version 330 core

out vec4 outColor;

uniform sampler2D uTexture;

void main() {
    outColor = texture(uTexture, gl_PointCoord);
    
    //outColor = vec4(1.0, 0.0, 0.0, 1.0);
}