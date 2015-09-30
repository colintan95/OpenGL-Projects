#version 330

in vec2 fTexCoord;
in vec4 fEyePos;
in vec3 fNormal;
in vec4 fLightPos;
in vec3 fConeDirection;

out vec4 outColor;

uniform sampler2D uTexSampler;

uniform vec3 uAmbient;
uniform vec3 uDiffuse;
uniform vec3 uSpecular;
uniform float uShininess;
uniform float uLightStrength;
uniform float uLinearAttenuation;

uniform float uConeLimit;
uniform float uSpotExponent;

void main() {
    
    vec4 baseColor = texture(uTexSampler, fTexCoord);
    
    float spec = 0.0;
    
    vec3 lightDirection = (fLightPos - fEyePos).xyz;
    
    float lightDist = length(lightDirection);
    
    float attenuation = 1.0 / (uLinearAttenuation * lightDist);
    
    vec3 light = normalize(lightDirection);
    vec3 norm = normalize(fNormal);
    vec3 eye = normalize(-fEyePos.xyz);
    
    vec3 coneDir = -normalize(fConeDirection);
    
    float coneDot = dot(light, coneDir);
    
    if (coneDot < uConeLimit) {
        attenuation = 0.0;
    }
    else {
        attenuation *= pow(coneDot, uSpotExponent);
    }
    
    float intensity = max(dot(norm, light), 0);
    
    if (intensity > 0) {
        vec3 h = normalize(light + eye);
        
        float dotSpec = max(dot(h,norm), 0);
        spec = pow(dotSpec, uShininess);
    }
    
    vec3 rgb = uAmbient * baseColor.rgb + intensity * uDiffuse * attenuation + spec * uSpecular * uLightStrength * attenuation;
    
    rgb = min(rgb, vec3(1.0));
    
    outColor = vec4(rgb, baseColor.a);
}