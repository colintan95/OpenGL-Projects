#version 330 core

in vec3 fViewPos;
in vec3 fViewNorm;
in vec3 fLightPos;

out vec4 outColor;

uniform samplerCube uCubemapTex;

uniform mat4 uViewMat;

uniform vec4 uAmbient;
uniform vec4 uDiffuse;
uniform vec4 uSpecular;
uniform float uShininess;

void main() {
    /*float spec = 0.0;
    
    vec3 light = normalize((fLightPos - fEyePos).xyz);
    vec3 norm = normalize(fNormal);
    vec3 eye = normalize(-fEyePos.xyz);
    
    float intensity = max(dot(norm, light), 0);
    
    if (intensity > 0) {
        vec3 h = normalize(light + eye);
        
        float dotSpec = max(dot(h,norm), 0);
        spec = pow(dotSpec, uShininess);
    }*/
    
    mat3 invView = mat3(inverse(uViewMat));
    
    vec3 texCoord = invView * reflect(fViewPos, normalize(fViewNorm));
    
    //outColor = spec * uSpecular + uAmbient * texture(uCubemapTex, texCoord) + intensity * uDiffuse;
    
    outColor = vec4(0.3, 0.2, 0.1, 1.0) + vec4(0.97, 0.83, 0.79, 0.0) * texture(uCubemapTex, texCoord);
}