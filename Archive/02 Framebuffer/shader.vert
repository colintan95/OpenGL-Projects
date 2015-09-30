#version 330

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec3 vNormal;

out vec3 fColor;

uniform mat4x4 uModelMat;
uniform mat4x4 uViewMat;
uniform mat4x4 uProjMat;

uniform vec3 uDirLight;

void main() {
    gl_Position = uProjMat * uViewMat * uModelMat * vPosition;
    
    mat3 normMat = transpose(inverse(mat3(uModelMat)));
    vec3 normal = normMat * vNormal;
    
    vec3 ambColor = vColor * 0.5;
    
    float diffDot = clamp(dot(normal, uDirLight), 0, 1);
    vec3 diffColor = vColor * 0.5 * diffDot;
    
    
    fColor = ambColor + diffColor;
}