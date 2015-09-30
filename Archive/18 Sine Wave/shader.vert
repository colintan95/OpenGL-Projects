#version 330

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;

out vec2 fTexCoord;

out vec3 fNormal;
out vec4 fEyePos;
out vec4 fLightPos;
out vec3 fConeDirection;
out vec3 fSkyNormal;
out vec4 fShadowCoord;

uniform mat4 uModelMat;
uniform mat4 uViewMat;
uniform mat4 uProjMat;

uniform mat4 uScaleBiasMat;
uniform mat4 uShadowMat;

uniform vec4 uLightPos;
uniform vec3 uConeDirection;

void main() {
    
    fEyePos = uViewMat * uModelMat * vPosition;
    
    fLightPos = uViewMat * uLightPos;
    
    gl_Position = uProjMat * uViewMat * uModelMat * vPosition;
    
    mat3 normMat = transpose(inverse(mat3(uViewMat * uModelMat)));
    vec3 normal = normMat * vNormal;
    
    mat3 viewNormMat = transpose(inverse(mat3(uViewMat)));

    fConeDirection = viewNormMat * uConeDirection;
    
    fNormal = normal;
    
    fTexCoord = vTexCoord;
    
    fSkyNormal = viewNormMat * vec3(0, 1, 0);
    
    vec4 shadowClip = uScaleBiasMat * uShadowMat * uModelMat * vPosition;
    
    fShadowCoord = shadowClip;
}