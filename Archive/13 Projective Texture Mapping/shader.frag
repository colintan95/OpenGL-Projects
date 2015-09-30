#version 330

in vec2 fTexCoord;
in vec4 fEyePos;
in vec3 fNormal;
in vec4 fLightPos;
in vec3 fConeDirection;
in vec3 fSkyNormal;
in vec4 fShadowCoord;
in vec4 fProjectorTexCoord;

out vec4 outColor;

uniform sampler2DShadow uDepthTexture;
uniform sampler2DShadow uProjectorDepthTexture;

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

uniform float uConeAngle;
uniform float uSpotExponent;

uniform vec3 uAmbientSky;
uniform vec3 uAmbientGround;

void main() {
    
    vec4 baseColor;
    vec4 projTextureColor = vec4(0.0);
    
    if (uIsTextured && fProjectorTexCoord.z > 0) {
        //baseColor = texture(uTexUnit, fTexCoord);
        projTextureColor = textureProj(uTexUnit, fProjectorTexCoord);
        float projShadow = textureProj(uProjectorDepthTexture, fProjectorTexCoord);
        projTextureColor *= projShadow;
        baseColor = uColor + projTextureColor;
    }
    else {
        baseColor = uColor;
    }
    
    //vec4 shadowCoord = fShadowCoord / fShadowCoord.w;
    
    //OpenGL handles perspective division
    float shadow = textureProj(uDepthTexture, fShadowCoord);
    
    float spec = 0.0;
    
    vec3 lightDirection = (fLightPos - fEyePos).xyz;
    
    float lightDist = length(lightDirection);
    
    float attenuation = 1.0 / (uConstAttenuation + uLinearAttenuation * lightDist);
    
    vec3 light = normalize(lightDirection);
    vec3 norm = normalize(fNormal);
    vec3 eye = normalize(-fEyePos.xyz);
    
    vec3 coneDir = -normalize(fConeDirection);
    
    
    
    float coneDot = dot(light, coneDir);
    
    if (coneDot < uConeAngle) {
        attenuation = 0.0;
    }
    else {
        attenuation *= pow(coneDot, uSpotExponent);
    }
    
    vec3 ambient = uAmbient;
    
    float intensity = max(dot(norm, light), 0);
    
    //Specular Lighting
    if (intensity > 0) {
        vec3 h = normalize(light + eye);
     
        float dotSpec = max(dot(h,norm), 0);
        spec = pow(dotSpec, uShininess);
    }
     
    vec3 rgb = ambient * baseColor.rgb + shadow * (intensity * uDiffuse * attenuation + spec * uSpecular * uSpecStrength * attenuation);
    
    //Hemisphere lighting
    /*vec3 skyNormal = normalize(fSkyNormal);
     float hemiDot = dot(skyNormal, norm);
     float hemiAlpha = hemiDot * 0.5 + 0.5;
     vec3 ambient = mix(uAmbientGround, uAmbientSky, hemiAlpha);*/
    
    //ambient = vec3(0.5, 0.5, 0.5);
    
    rgb = min(rgb, vec3(1.0));
    
    outColor = vec4(rgb, baseColor.a);
}