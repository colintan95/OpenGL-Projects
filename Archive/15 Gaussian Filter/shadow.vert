#version 330 core

uniform mat4 uModelMat;
uniform mat4 uViewMat;
uniform mat4 uProjMat;

layout (location = 0) in vec4 vPosition;

out float fDepth;

void main() {
    gl_Position = uProjMat * uViewMat * uModelMat * vPosition;
    
    fDepth = (uViewMat * uModelMat * vPosition).z;
}