#version 330 core

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;

out vec2 vfTexCoord;
out vec3 vfNormal;
out vec4 vfEyePos;
out vec4 vfLightPos;
out vec4 vfShadowCoord;

uniform float uWavenumber;
uniform float uVelocity;
uniform float uAmplitude;
uniform float uTime;

uniform mat4 uModelMat;
uniform mat4 uViewMat;
uniform mat4 uProjMat;

uniform mat4 uScaleBiasMat;
uniform mat4 uShadowMat;

uniform vec4 uLightPos;

void main() {
    
    vec4 pos = vPosition;
    float u = uWavenumber * vPosition.x - uVelocity * uTime;
    pos.y = uAmplitude * sin(u);
    
    vec3 norm = vec3(0.0);
    norm.xy = normalize(vec2(- uWavenumber * uAmplitude * cos(u), 1.0));
    
    vfEyePos = uViewMat * uModelMat * pos;
    vfLightPos = uViewMat * uLightPos;
    gl_Position = uProjMat * uViewMat * uModelMat * pos;
    
    mat3 normMat = transpose(inverse(mat3(uViewMat * uModelMat)));
    vec3 normal = normMat * norm;
    
    mat3 viewNormMat = transpose(inverse(mat3(uViewMat)));
    
    vfNormal = normal;
    
    vfTexCoord = vTexCoord;
    
    vec4 shadowClip = uScaleBiasMat * uShadowMat * uModelMat * vPosition;
    
    vfShadowCoord = shadowClip;
}