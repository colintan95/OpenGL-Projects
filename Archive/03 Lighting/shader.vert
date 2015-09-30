#version 330

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec3 vNormal;

out vec2 fTexCoord;

out vec3 fNormal;
out vec4 fEyePos;
out vec4 fLightPos;

uniform mat4 uModelMat;
uniform mat4 uViewMat;
uniform mat4 uProjMat;

uniform vec4 uLightPos;

void main() {
    fEyePos = uViewMat * uModelMat * vPosition;
    
    fLightPos = uViewMat * uLightPos;
    
    gl_Position = uProjMat * uViewMat * uModelMat * vPosition;
    
    mat3 normMat = transpose(inverse(mat3(uViewMat * uModelMat)));
    vec3 normal = normMat * vNormal;
    
    fNormal = normal;
    
    fTexCoord = vTexCoord;
}