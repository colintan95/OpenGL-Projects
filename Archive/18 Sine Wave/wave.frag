#version 330 core

in vec2 vfTexCoord;
in vec4 vfEyePos;
in vec3 vfNormal;
in vec4 vfLightPos;
in vec4 vfShadowCoord;

out vec4 fColor;

uniform sampler2DShadow uDepthTexture;

uniform sampler2D uTexUnit;
uniform bool uIsTextured;

uniform vec4 uColor;

uniform vec3 uAmbient;
uniform vec3 uDiffuse;
uniform vec3 uSpecular;
uniform float uShininess;
uniform float uSpecStrength;

uniform float uConstAttenuation;
uniform float uLinearAttenuation;

vec4 PhongShading() {
    vec4 baseColor;
    
    if (uIsTextured) {
        baseColor = texture(uTexUnit, vfTexCoord);
    }
    else {
        baseColor = uColor;
    }
    
    //OpenGL handles perspective division
    float shadow = textureProj(uDepthTexture, vfShadowCoord);
    
    float spec = 0.0;
    
    vec3 lightDirection = (vfLightPos - vfEyePos).xyz;
    
    float lightDist = length(lightDirection);
    
    float attenuation = 1.0 / (uConstAttenuation + uLinearAttenuation * lightDist);
    
    vec3 light = normalize(lightDirection);
    vec3 norm = normalize(vfNormal);
    vec3 eye = normalize(-vfEyePos.xyz);

    vec3 ambient = uAmbient;
    
    float intensity = max(dot(norm, light), 0);
    
    //Specular Lighting
    if (intensity > 0) {
        vec3 h = normalize(light + eye);
        
        float dotSpec = max(dot(h,norm), 0);
        spec = pow(dotSpec, uShininess);
    }
    
    vec3 rgb = ambient * baseColor.rgb + shadow * (intensity * uDiffuse * attenuation + spec * uSpecular * uSpecStrength * attenuation);
    
    rgb = min(rgb, vec3(1.0));
    
    return vec4(rgb, baseColor.a);
}

void main() {
    fColor = PhongShading();
}