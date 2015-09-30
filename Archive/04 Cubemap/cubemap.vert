#version 330 core

layout(location = 0) in vec3 vPosition;

out vec3 fTexCoord;

uniform mat4 uModelMat;
uniform mat4 uViewMat;
uniform mat4 uProjMat;

void main() {
    vec4 position = vec4(vPosition.xyz, 1.0);
    
    fTexCoord = vPosition;
    
    gl_Position = uProjMat * uViewMat * uModelMat * position;
}