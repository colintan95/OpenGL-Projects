#version 330

layout(location = 0) in vec4 vPosition;
layout(location = 2) in vec3 vNormal;

out vec3 FragColor;

out vec4 position;

uniform mat4x4 viewMat;
uniform mat4x4 projMat;

uniform vec3 dirLight;

uniform samplerBuffer colorTbo;
uniform samplerBuffer modelMatTbo;

void main() {
    vec3 color = texelFetch(colorTbo, gl_InstanceID).rgb;
    
    vec4 matCol1 = texelFetch(modelMatTbo, gl_InstanceID * 4);
    vec4 matCol2 = texelFetch(modelMatTbo, gl_InstanceID * 4 + 1);
    vec4 matCol3 = texelFetch(modelMatTbo, gl_InstanceID * 4 + 2);
    vec4 matCol4 = texelFetch(modelMatTbo, gl_InstanceID * 4 + 3);
    mat4 modelMat = mat4(matCol1, matCol2, matCol3, matCol4);
    
    gl_Position = projMat * viewMat * modelMat * vPosition;
    
    mat3 normMat = transpose(inverse(mat3(modelMat)));
    vec3 normal = normMat * vNormal;
    
    vec3 ambColor = color * 0.5;
    float dirDot = clamp(dot(normal, dirLight), 0, 1);
    FragColor = ambColor + (color * 0.5) * dirDot;

    position = vPosition;
}