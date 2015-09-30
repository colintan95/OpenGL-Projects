#version 330

in vec2 fTexCoord;
in vec4 fEyePos;
in vec3 fNormal;
in vec4 fLightPos;

out vec4 outColor;

uniform sampler2D uTexSampler;

uniform vec4 uAmbient;
uniform vec4 uDiffuse;
uniform vec4 uSpecular;
uniform float uShininess;

void main() {
    
    float spec = 0.0;
    
    vec3 light = normalize((fLightPos - fEyePos).xyz);
    vec3 norm = normalize(fNormal);
    vec3 eye = normalize(-fEyePos.xyz);
    
    float intensity = max(dot(norm, light), 0);
    
    if (intensity > 0) {
        vec3 h = normalize(light + eye);
        
        float dotSpec = max(dot(h,norm), 0);
        spec = pow(dotSpec, uShininess);
    }
    
    outColor = spec * uSpecular + uAmbient * texture(uTexSampler, fTexCoord) + intensity * uDiffuse;
}