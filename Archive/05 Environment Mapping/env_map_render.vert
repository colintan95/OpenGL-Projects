#version 330 core

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormal;

out vec3 fViewPos;
out vec3 fViewNorm;
out vec3 fLightPos;

uniform mat4 uModelMat;
uniform mat4 uViewMat;
uniform mat4 uProjMat;

uniform vec4 uLightPos;

void main() {
    fViewPos = (uViewMat * uModelMat * vPosition).xyz;
    
    gl_Position = uProjMat * uViewMat * uModelMat * vPosition;
    
    mat3 normMat = transpose(inverse(mat3(uViewMat * uModelMat)));
    vec3 normal = normMat * vNormal;
    
    fViewNorm = normal;
    
    fLightPos = (uViewMat * uLightPos).xyz;
}