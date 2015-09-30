#version 330 core

in vec2 vfTexCoord;

out vec4 fColor;

uniform sampler2D uTexture;
uniform float uWeights[5];
uniform int uPass;

uniform int uScreenWidth;
uniform int uScreenHeight;

// Applies one-dimensional gaussian function to a column of 9 pixels
vec4 pass1() {
    
    float dy = 1.0 / float(uScreenHeight);
    
    vec4 sum = texture(uTexture, vfTexCoord) * uWeights[0]; // Pixel at row l
    
    
    // Sum in both directions (-dy, dy), with distance of 4 pixels in each direction
    for (int i = 1; i < 5; i++) {
        sum += texture(uTexture, vfTexCoord + vec2(0.0, i) * dy) * uWeights[i]; // Pixel at row l + i
        sum += texture(uTexture, vfTexCoord - vec2(0.0, i) * dy) * uWeights[i]; // Pixel at row l - i
    }
    
    return sum;
}

// Using texture produced from pass1, apply the gaussian function to row of 9 pixels
vec4 pass2() {

    float dx = 1.0 / float(uScreenWidth);
    
    vec4 sum = texture(uTexture, vfTexCoord) * uWeights[0]; // Pixel at column m
    
    // Sum in both directions (-dx, dx), with distance of 4 pixels in each direction
    for (int i = 1; i < 5; i++) {
        sum += texture(uTexture, vfTexCoord + vec2(i, 0.0) * dx) * uWeights[i]; // Pixel at column m + i
        sum += texture(uTexture, vfTexCoord - vec2(i, 0.0) * dx) * uWeights[i]; // Piexl at column m - i
    }
    
    return sum;
}


// Applies a 9x9 gaussian filter

void main() {
    
    // Uniform uPass determines which pass to use
    if (uPass == 1) {
        fColor = pass1();
    }
    else if (uPass == 2) {
        fColor = pass2();
    }
    else {
        fColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
}