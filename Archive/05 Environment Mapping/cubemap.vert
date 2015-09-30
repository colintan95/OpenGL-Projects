#version 330 core

layout(location = 0) in vec4 vPosition;

out vec3 fTexCoord;

uniform mat4 uModelMat;
uniform mat4 uViewMat;
uniform mat4 uProjMat;

void main() {
    vec4 position = vPosition;
    
    fTexCoord = vPosition.xyz;
    
    gl_Position = uProjMat * uViewMat * uModelMat * position;
}