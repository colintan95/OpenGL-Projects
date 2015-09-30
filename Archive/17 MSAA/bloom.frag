#version 330 core

in vec2 vfTexCoord;

out vec4 fColor;

uniform sampler2D uRenderTexture;
uniform sampler2D uTexture;
uniform float uWeights[20];
uniform float uLumThreshold;
uniform int uPass;

uniform int uScreenWidth;
uniform int uScreenHeight;

float luma(vec3 color) {
    return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}

// Discards fragments with luminance less than uLumThreshold
vec4 pass1() {
    vec4 tex = texture(uRenderTexture, vfTexCoord);
    
    float factor = clamp(luma(tex.rgb) - uLumThreshold, 0.0, 1.0) / (1.0 - uLumThreshold);
    
    return vec4(tex.rgb * factor, tex.a);
}

// Vertical pass of Gaussian Blur
vec4 pass2() {
    
    float dy = 1.0 / float(uScreenHeight);
    
    vec4 sum = texture(uTexture, vfTexCoord) * uWeights[0]; // Pixel at row l
    
    
    // Sum in both directions (-dy, dy)
    for (int i = 1; i < 20; i++) {
        sum += texture(uTexture, vfTexCoord + vec2(0.0, i) * dy) * uWeights[i]; // Pixel at row l + i
        sum += texture(uTexture, vfTexCoord - vec2(0.0, i) * dy) * uWeights[i]; // Pixel at row l - i
    }
    
    return sum;
}

// Horizontal pass of Gaussian Blur. Blur added to original rendered image
vec4 pass3() {
    
    float dx = 1.0 / float(uScreenWidth);
    
    vec4 sum = texture(uTexture, vfTexCoord) * uWeights[0]; // Pixel at column m
    
    // Sum in both directions (-dx, dx)
    for (int i = 1; i < 20; i++) {
        sum += texture(uTexture, vfTexCoord + vec2(i, 0.0) * dx) * uWeights[i]; // Pixel at column m + i
        sum += texture(uTexture, vfTexCoord - vec2(i, 0.0) * dx) * uWeights[i]; // Piexl at column m - i
    }
    
    vec4 tex = texture(uRenderTexture, vfTexCoord);
    
    return tex + sum;
}


void main() {
    
    // Uniform uPass determines which pass to use
    if (uPass == 1) {
        fColor = pass1();
    }
    else if (uPass == 2) {
        fColor = pass2();
    }
    else if (uPass == 3) {
        fColor = pass3();
    }
    else {
        fColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
}