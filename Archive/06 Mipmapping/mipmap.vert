#version 330 core

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec2 vTexCoord;

uniform mat4 uModelMat;
uniform mat4 uViewMat;
uniform mat4 uProjMat;

out vec2 fTexCoord;

void main() {
    gl_Position = uProjMat * uViewMat * uModelMat * vPosition;
    
    fTexCoord = vTexCoord;
}