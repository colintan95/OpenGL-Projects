#version 330 core

in vec4 vPosition;

uniform mat4 uModelMat;
uniform mat4 uViewMat;
uniform mat4 uProjMat;

void main() {
    vec4 pos = uProjMat * uViewMat * uModelMat * vPosition;
    vec4 viewPos = uViewMat * uModelMat * vPosition;
    float dist = length(viewPos.xyz);
    gl_PointSize = 5.0 / (-viewPos.z) * 128.0; //Sprite is of size 128 when at z = 5
    gl_Position = pos;
}