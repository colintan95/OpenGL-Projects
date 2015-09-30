#version 330 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vVelocity;
layout(location = 2) in float vStartTime;
layout(location = 3) in vec3 vInitVelocity;

// Outputs to transform feedback buffer
out vec3 trPosition;
out vec3 trVelocity;
out float trStartTime;

out float vfAlpha;

uniform float uTime;
uniform float uFrametime; // Elapsed time from previous frame
uniform float uLifetime;
uniform vec3 uAcceleration;
uniform float uMinParticleSize;
uniform float uMaxParticleSize;

uniform mat4 uModelMat;
uniform mat4 uViewMat;
uniform mat4 uProjMat;

uniform int uPass;

void pass1() {
    trPosition = vPosition;
    trVelocity = vVelocity;
    trStartTime = vStartTime;
    
    // Particle appears after start time
    if (uTime >= trStartTime) {
        float age = uTime - trStartTime;
        
        // Reuse particles when their lifetime is reached
        if (age > uLifetime) {
            trPosition = vec3(0.0);
            trVelocity = vInitVelocity;
            trStartTime = uTime;
        }
        else {
            // Update particle attributes
            trPosition += trVelocity * uFrametime;
            trVelocity += uAcceleration * uFrametime;
        }
    }
}

void pass2() {
    float age = uTime - vStartTime;
    vfAlpha = 1.0 - age / uLifetime; // Particle fades as it reaches its lifetime
    gl_PointSize = mix(uMinParticleSize, uMaxParticleSize, age / uLifetime);
    gl_Position = uProjMat * uViewMat * uModelMat * vec4(vPosition, 1.0);
}

void main() {
    if (uPass == 1) {
        pass1();
    }
    else if (uPass == 2) {
        pass2();
    }
}