#version 330 core

layout(location = 0) in float vStartTime;
layout(location = 1) in vec3 vVelocity;

out float vfAlpha;

uniform float uTime;
uniform float uLifetime;
uniform vec3 uAcceleration;

uniform mat4 uModelMat;
uniform mat4 uViewMat;
uniform mat4 uProjMat;

void main() {
    vec3 pos = vec3(0.0);
    float alpha = 0.f;
    
    // Particles only appear after start time
    if (uTime > vStartTime) {
        float t = uTime - vStartTime;
        
        if (t < uLifetime) {
            pos = vVelocity * t + 0.5 * uAcceleration * t * t;
            alpha = 1.0 - t / uLifetime; // Particle fades as it reaches the end of its lifetime
        }
    }
    
    vfAlpha = alpha;
    
    gl_Position = uProjMat * uViewMat * uModelMat * vec4(pos, 1.0);
}