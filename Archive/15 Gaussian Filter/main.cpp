//
//  main.cpp
//  Template
//
//  Created by Colin Tan on 3/4/15.
//  Copyright (c) 2015 Colin Tan. All rights reserved.
//

#include <iostream>

#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#include <OpenGL/glu.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define GLFW_INCLUDE_GLCOREARB
#include "glfw3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <math.h>
#include <vector>

#include "Shader.h"
#include "Model.h"

//Configurations

const int GL_VERSION_MAJOR = 3;
const int GL_VERSION_MINOR = 2;

const int WIN_WIDTH = 640;
const int WIN_HEIGHT = 480;

const char* WIN_TITLE = "OpenGL Program";

const int SHADOW_DEPTH_TEXTURE_SIZE = 1280;
const float SHADOW_FRUSTRUM_DEPTH = 100.f;

struct SceneObject {
    glm::mat4 modelMat;
    
    glm::vec3 ambientColor;
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
    
    float shininess;
    float specStrength;

    GLuint glPosBuf;
    GLuint glTexCoordBuf;
    GLuint glNormBuf;
    int vertCount;
    
    GLuint glTexture;
    GLuint glTexUnit;
    
    bool isTextured;
    
    glm::vec4 objColor = {1.f, 0.f, 0.f, 1.f}; //Color of object if isTextured is false
};

std::vector<SceneObject> gSceneGraph;

//GLFW User Variables

int gScreenWidth;
int gScreenHeight;

//OpenGL User-defined Global Variables

GLuint gRenderProg;
GLuint gRenderVS;
GLuint gRenderFS;

GLuint gVao;

GLuint gPosBuf;
GLuint gTexCoordBuf;
GLuint gNormBuf;
GLuint gPosIndexBuf;

GLuint gFloorPosBuf;
GLuint gFloorTexCoordBuf;
GLuint gFloorNormBuf;

GLuint gPixelBuf;
GLuint gTexture;

GLint gPosLoc;
GLint gTexCoordLoc;
GLint gNormLoc;

GLint gProjectorMatLoc;

GLint gModelMatLoc;
GLint gViewMatLoc;
GLint gProjMatLoc;
GLint gScaleBiasMatLoc;
GLint gShadowMatLoc;
GLint gTexUnitLoc;
GLint gIsTexturedLoc;
GLint gColorLoc;

GLint gAmbientLoc;
GLint gDiffuseLoc;
GLint gSpecularLoc;
GLint gShininessLoc;
GLint gLightPosLoc;
GLint gSpecStrengthLoc;

GLint gConstAttenuationLoc;
GLint gLinearAttenuationLoc;

GLint gConeDirectionLoc;
GLint gConeAngleLoc;
GLint gSpotExponentLoc;

GLint gAmbientSkyLoc;
GLint gAmbientGroundLoc;

GLint gDepthTextureLoc;
GLint gProjectorDepthTextureLoc;

GLuint gShadowProg;
GLuint gShadowVS;
GLuint gShadowFS;

GLuint gShadowTexture;
GLuint gShadowFbo;

GLuint gShadowVao;

GLint gShadowPosLoc;

GLint gShadowModelMatLoc;
GLint gShadowViewMatLoc;
GLint gShadowProjMatLoc;

glm::mat4 gShadowViewMat;
glm::mat4 gShadowProjMat;

GLuint gFilterProg;
GLuint gFilterVS;
GLuint gFilterFS;

GLuint gFilterVao;

GLuint gFilterPosBuf;
GLuint gFilterTexCoordBuf;

GLint gFilterPosBufLoc;
GLint gFilterTexCoordBufLoc;

GLint gFilterTextureLoc;
GLint gFilterWidthLoc;
GLint gFilterHeightLoc;
GLint gFilterPassLoc;
GLint gFilterWeightsLoc;

GLuint gFilterSceneFbo;
GLuint gFilterSceneRbo;
GLuint gFilterSceneTexture;

GLuint gFilterPassFbo;
GLuint gFilterPassTexture;

//OpenGL Constant Variables

const GLfloat PROJ_NEAR = 0.1f;
const GLfloat PROJ_FAR = 1000.f;

//Other User-defined Global Variables

Model teapot;

glm::mat4 gModelMat(1.f);

unsigned char* gTexImage;

SceneObject gFloorObj;
SceneObject gTeapotObj;

float gauss(int i, float sigma) {
    float exp = - (i * i) / (2.f * sigma * sigma);
    
    float fract = 1.f / sqrt(2 * M_PI * sigma * sigma);
    
    return fract * pow(M_E, exp);
}

//OpenGL Initialization Logic
void glInit() {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.f, -1.0f);
    
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
    glm::vec3 coneDir = {0.f, -1.f, -1.f};
    glm::vec4 lightPos = {0.f, 30.f, 30.f, 1.f};
    
    glm::mat4 scaleBiasMat = {{0.5f, 0.f, 0.f, 0.f}, {0.f, 0.5f, 0.f, 0.f}, {0.f, 0.f, 0.5f, 0.f}, {0.5f, 0.5f, 0.5f, 1.f}};
    
    
    GLuint TeapotTexture;
    
    int imgWidth, imgHeight, imgComp;
    gTexImage = stbi_load("texture.jpg", &imgWidth, &imgHeight, &imgComp, STBI_rgb);
    glGenTextures(1, &TeapotTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, TeapotTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, gTexImage);
    glActiveTexture(GL_TEXTURE0);
    
    //Shadow Map Creation
    
    gShadowProg = glCreateProgram();
    gShadowVS = CreateShader(GL_VERTEX_SHADER, "shadow.vert");
    gShadowFS = CreateShader(GL_FRAGMENT_SHADER, "shadow.frag");
    glAttachShader(gShadowProg, gShadowVS);
    glAttachShader(gShadowProg, gShadowFS);
    glLinkProgram(gShadowProg);
    glUseProgram(gShadowProg);
    
    glGenFramebuffers(1, &gShadowFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, gShadowFbo);
    
    glDrawBuffer(GL_NONE);
    
    glGenTextures(1, &gShadowTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gShadowTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_DEPTH_TEXTURE_SIZE, SHADOW_DEPTH_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glActiveTexture(GL_TEXTURE0);
    
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, gShadowTexture, 0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    gShadowViewMat = glm::lookAt(glm::vec3(lightPos), glm::vec3(lightPos) + coneDir, glm::vec3(0, 1, 0));
    gShadowProjMat = glm::frustum(-1.f, 1.f, -1.f, 1.f, 1.f, SHADOW_FRUSTRUM_DEPTH);
    
    gShadowModelMatLoc = glGetUniformLocation(gShadowProg, "uModelMat");
    
    gShadowViewMatLoc = glGetUniformLocation(gShadowProg, "uViewMat");
    glUniformMatrix4fv(gShadowViewMatLoc, 1, GL_FALSE, glm::value_ptr(gShadowViewMat));
    
    gShadowProjMatLoc = glGetUniformLocation(gShadowProg, "uProjMat");
    glUniformMatrix4fv(gShadowProjMatLoc, 1, GL_FALSE, glm::value_ptr(gShadowProjMat));
    
    glGenVertexArrays(1, &gShadowVao);
    glBindVertexArray(gShadowVao);
    
    gShadowPosLoc = glGetAttribLocation(gShadowProg, "vPosition");
    glEnableVertexAttribArray(gShadowPosLoc);
    
    //Program Creation
    
    gRenderProg = glCreateProgram();
    gRenderVS = CreateShader(GL_VERTEX_SHADER, "shader.vert");
    gRenderFS = CreateShader(GL_FRAGMENT_SHADER, "shader.frag");
    glAttachShader(gRenderProg, gRenderVS);
    glAttachShader(gRenderProg, gRenderFS);
    glLinkProgram(gRenderProg);
    glUseProgram(gRenderProg);
    
    
    //Texture
  
    //Uniforms
    
    gModelMatLoc = glGetUniformLocation(gRenderProg, "uModelMat");
    gModelMat = glm::rotate(glm::mat4(1.f), -1.57f, glm::vec3(1.f, 0.f, 0.f));
    glUniformMatrix4fv(gModelMatLoc, 1, GL_FALSE, glm::value_ptr(gModelMat));
    
    gViewMatLoc = glGetUniformLocation(gRenderProg, "uViewMat");
    glm::mat4 viewMat = glm::mat4(1.f);
    viewMat = glm::rotate(glm::mat4(1.f), 0.7f, glm::vec3(1.f, 0.f, 0.f)) * viewMat;
    viewMat = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -50.f)) * viewMat;
    glUniformMatrix4fv(gViewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
    
    gProjMatLoc = glGetUniformLocation(gRenderProg, "uProjMat");
    glm::mat4 projMat = glm::perspective(45.f, (float)gScreenWidth / (float)gScreenHeight, PROJ_NEAR, PROJ_FAR);
    glUniformMatrix4fv(gProjMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));
    
    gScaleBiasMatLoc = glGetUniformLocation(gRenderProg, "uScaleBiasMat");
    glUniformMatrix4fv(gScaleBiasMatLoc, 1, GL_FALSE, glm::value_ptr(scaleBiasMat));
    
    gShadowMatLoc = glGetUniformLocation(gRenderProg, "uShadowMat");
    glm::mat4 shadowMat = gShadowProjMat * gShadowViewMat;
    glUniformMatrix4fv(gShadowMatLoc, 1, GL_FALSE, glm::value_ptr(shadowMat));
    
    gTexUnitLoc = glGetUniformLocation(gRenderProg, "uTexUnit");
    glUniform1i(gTexUnitLoc, 0);
    
    gIsTexturedLoc = glGetUniformLocation(gRenderProg, "uIsTextured");
    glUniform1i(gIsTexturedLoc, 1);
    
    gColorLoc = glGetUniformLocation(gRenderProg, "uColor");
    glm::vec4 color = {1.0f, 0.f, 0.f, 1.f};
    glUniform4fv(gColorLoc, 1, glm::value_ptr(color));
    
    gAmbientLoc = glGetUniformLocation(gRenderProg, "uAmbient");
    glm::vec3 ambVec = {0.3f, 0.3f, 0.3f};
    glUniform3fv(gAmbientLoc, 1, glm::value_ptr(ambVec));
    
    gDiffuseLoc = glGetUniformLocation(gRenderProg, "uDiffuse");
    glm::vec3 difVec = {0.5f, 0.f, 0.f};
    glUniform3fv(gDiffuseLoc, 1, glm::value_ptr(difVec));
    
    gSpecularLoc = glGetUniformLocation(gRenderProg, "uSpecular");
    glm::vec3 specVec = {1.f, 1.f, 1.f};
    glUniform3fv(gSpecularLoc, 1, glm::value_ptr(specVec));
    
    gShininessLoc = glGetUniformLocation(gRenderProg, "uShininess");
    float shininess = 20.0f;
    glUniform1f(gShininessLoc, shininess);
    
    gLightPosLoc = glGetUniformLocation(gRenderProg, "uLightPos");
    glUniform4fv(gLightPosLoc, 1, glm::value_ptr(lightPos));
    
    gSpecStrengthLoc = glGetUniformLocation(gRenderProg, "uSpecStrength");
    float strength = 3.f;
    glUniform1f(gSpecStrengthLoc, strength);
    
    
    gConstAttenuationLoc = glGetUniformLocation(gRenderProg, "uConstAttenuation");
    float constAttenuation = 1.f;
    glUniform1f(gConstAttenuationLoc, constAttenuation);
    
    gLinearAttenuationLoc = glGetUniformLocation(gRenderProg, "uLinearAttenuation");
    float linearAttenuation = 0.05f;
    glUniform1f(gLinearAttenuationLoc, linearAttenuation);
    
    gConeDirectionLoc = glGetUniformLocation(gRenderProg, "uConeDirection");
    glUniform3fv(gConeDirectionLoc, 1, glm::value_ptr(coneDir));
    
    gConeAngleLoc = glGetUniformLocation(gRenderProg, "uConeAngle");
    float coneAngle = 0.8f;
    glUniform1f(gConeAngleLoc, coneAngle);
    
    gSpotExponentLoc = glGetUniformLocation(gRenderProg, "uSpotExponent");
    float spotExponent = 1.0f;
    glUniform1f(gSpotExponentLoc, spotExponent);
    
    gAmbientSkyLoc = glGetUniformLocation(gRenderProg, "uAmbientSky");
    glm::vec3 ambientSky = {0.3f, 0.3f, 0.3f};
    glUniform3fv(gAmbientSkyLoc, 1, glm::value_ptr(ambientSky));
    
    gAmbientGroundLoc = glGetUniformLocation(gRenderProg, "uAmbientGround");
    glm::vec3 ambientGround = {0.1f, 0.1f, 0.1f};
    glUniform3fv(gAmbientGroundLoc, 1, glm::value_ptr(ambientGround));
    
    gDepthTextureLoc = glGetUniformLocation(gRenderProg, "uDepthTexture");
    glUniform1i(gDepthTextureLoc, 2);
    
    gProjectorDepthTextureLoc = glGetUniformLocation(gRenderProg, "uProjectorDepthTexture");
    glUniform1i(gProjectorDepthTextureLoc, 3);

    //Vertex Array Object
    
    glGenVertexArrays(1, &gVao);
    glBindVertexArray(gVao);
    

    //Vertex Attribute Specification
    
    gPosLoc = glGetAttribLocation(gRenderProg, "vPosition");
    glEnableVertexAttribArray(gPosLoc);
    
    gTexCoordLoc = glGetAttribLocation(gRenderProg, "vTexCoord");
    glEnableVertexAttribArray(gTexCoordLoc);

    gNormLoc = glGetAttribLocation(gRenderProg, "vNormal");
    glEnableVertexAttribArray(gNormLoc);
    
    //Floor
    
    gFloorObj.modelMat = glm::mat4(1.f);
    
    GLuint floorBuffers[3];
    glGenBuffers(3, &floorBuffers[0]);
    
    glBindBuffer(GL_ARRAY_BUFFER, floorBuffers[0]);
    glm::vec4 floorPosData[] = {{-60.f, 0.f, -30.f, 1.f}, {-60, 0.f, 30.f, 1.f}, {60.f, 0.f, -30.f, 1.f}, {60.f, 0.f, -30.f, 1.f}, {-60.f, 0.f, 30.f, 1.f}, {60.f, 0.f, 30.f, 1.f}};
    glBufferData(GL_ARRAY_BUFFER, 4 * 6 * sizeof(float), &floorPosData[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, floorBuffers[1]);
    glm::vec2 floorTexCoordData[] = {{0.f, 0.f}, {0.f, 1.f}, {1.f, 0.f}, {1.f, 0.f}, {0.f, 1.f}, {1.f, 1.f}};
    glBufferData(GL_ARRAY_BUFFER, 2 * 6 * sizeof(float), &floorTexCoordData[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, floorBuffers[2]);
    glm::vec3 floorNormData[] = {{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}};
    glBufferData(GL_ARRAY_BUFFER, 3 * 6 * sizeof(float), &floorNormData[0], GL_STATIC_DRAW);
    
    gFloorObj.glPosBuf = floorBuffers[0];
    gFloorObj.glTexCoordBuf = floorBuffers[1];
    gFloorObj.glNormBuf = floorBuffers[2];
    
    gFloorObj.vertCount = 6;
    
    gFloorObj.ambientColor = {0.3f, 0.3f, 0.3f};
    gFloorObj.diffuseColor = {0.5f, 0.5f, 0.5f};
    gFloorObj.specularColor = {1.f, 1.f, 1.f};
    
    gFloorObj.shininess = 20.f;
    gFloorObj.specStrength = 0.f;
    
    gFloorObj.isTextured = false;
    gFloorObj.objColor = {0.5f, 0.5f, 0.5f, 1.f};
    
    gFloorObj.glTexture = TeapotTexture;
    gFloorObj.glTexUnit = 0;
    
    //Teapot
    
    gTeapotObj.modelMat = glm::rotate(glm::mat4(1.f), -1.57f, glm::vec3(1.f, 0.f, 0.f));
    
    GLuint TeapotBuffers[3];
    glGenBuffers(3, &TeapotBuffers[0]);
    
    glBindBuffer(GL_ARRAY_BUFFER, TeapotBuffers[0]);
    glBufferData(GL_ARRAY_BUFFER, teapot.m_Info.vertCount * 4 * sizeof(float), &teapot.m_Info.v[0], GL_STREAM_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, TeapotBuffers[1]);
    glBufferData(GL_ARRAY_BUFFER, teapot.m_Info.vertCount * 2 * sizeof(float), &teapot.m_Info.vt[0], GL_STREAM_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, TeapotBuffers[2]);
    glBufferData(GL_ARRAY_BUFFER, teapot.m_Info.vertCount * 3 * sizeof(float), &teapot.m_Info.vn[0], GL_STREAM_DRAW);
    
    gTeapotObj.glPosBuf = TeapotBuffers[0];
    gTeapotObj.glTexCoordBuf = TeapotBuffers[1];
    gTeapotObj.glNormBuf = TeapotBuffers[2];
    
    gTeapotObj.vertCount = teapot.m_Info.vertCount;
    
    gTeapotObj.ambientColor = {0.3f, 0.3f, 0.3f};
    gTeapotObj.diffuseColor = {0.8f, 0.f, 0.f};
    gTeapotObj.specularColor = {1.f, 1.f, 1.f};
    
    gTeapotObj.shininess = 20.f;
    gTeapotObj.specStrength = 5.f;
    
    gTeapotObj.isTextured = true;
    gTeapotObj.objColor = {0.1f, 0.f, 0.f, 1.f};
    
    gTeapotObj.glTexture = TeapotTexture;
    gTeapotObj.glTexUnit = 1;
    
    gSceneGraph.push_back(gTeapotObj);
    gSceneGraph.push_back(gFloorObj);
    
    gFilterProg = glCreateProgram();
    gFilterVS = CreateShader(GL_VERTEX_SHADER, "gaussian_blur.vert");
    gFilterFS = CreateShader(GL_FRAGMENT_SHADER, "gaussian_blur.frag");
    glAttachShader(gFilterProg, gFilterVS);
    glAttachShader(gFilterProg, gFilterFS);
    glLinkProgram(gFilterProg);
    glUseProgram(gFilterProg);
    
    
    // Swap textures in shader by binding texture to tex unit 3 (GL_TEXTURE3)
    
    gFilterTextureLoc = glGetUniformLocation(gFilterProg, "uTexture");
    glUniform1i(gFilterTextureLoc, 3);
    
    gFilterWidthLoc = glGetUniformLocation(gFilterProg, "uScreenWidth");
    glUniform1i(gFilterWidthLoc, gScreenWidth * 2);
    
    gFilterHeightLoc = glGetUniformLocation(gFilterProg, "uScreenHeight");
    glUniform1i(gFilterHeightLoc, gScreenHeight * 2);
    
    gFilterPassLoc = glGetUniformLocation(gFilterProg, "uPass");
    glUniform1i(gFilterPassLoc, 1);
    
    
    // Calculates the weights of the gaussian function
    
    float weights[5];
    float sum;
    float sigma = 4.f;
    
    weights[0] = gauss(0, sigma);
    sum += weights[0];
    
    for (int i = 1; i < 5; i++) {
        weights[i] = gauss(i, sigma);
        sum += 2 * weights[i];
    }
    
    for (int i = 0; i < 5; i++) {
        weights[i] = weights[i] / sum; // Normalizes weights: sum of weights equals 1
    }
    
    gFilterWeightsLoc = glGetUniformLocation(gFilterProg, "uWeights");
    glUniform1fv(gFilterWeightsLoc, 5, weights);
    
    glGenBuffers(1, &gFilterPosBuf);
    glBindBuffer(GL_ARRAY_BUFFER, gFilterPosBuf);
    glm::vec4 filterPosData[] = {{-1.f, 1.f, 0.f, 1.f}, {-1.f, -1.f, 0.f, 1.f}, {1.f, 1.f, 0.f, 1.f}, {1.f, 1.f, 0.f, 1.f}, {-1.f, -1.f, 0.f, 1.f}, {1.f, -1.f, 0.f, 1.f}};
    glBufferData(GL_ARRAY_BUFFER, 6 * 4 * sizeof(float), &filterPosData[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &gFilterTexCoordBuf);
    glBindBuffer(GL_ARRAY_BUFFER, gFilterTexCoordBuf);
    glm::vec2 filterTexCoordData[] = {{0.f, 1.f}, {0.f, 0.f}, {1.f, 1.f}, {1.f, 1.f}, {0.f, 0.f}, {1.f, 0.f}};
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), &filterTexCoordData[0], GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &gFilterVao);
    glBindVertexArray(gFilterVao);
    
    gFilterPosBufLoc = glGetAttribLocation(gFilterProg, "vPosition");
    glEnableVertexAttribArray(gFilterPosBufLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gFilterPosBuf);
    glVertexAttribPointer(gFilterPosBufLoc, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    
    gFilterTexCoordBufLoc = glGetAttribLocation(gFilterProg, "vTexCoord");
    glEnableVertexAttribArray(gFilterTexCoordBufLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gFilterTexCoordBuf);
    glVertexAttribPointer(gFilterTexCoordBufLoc, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    
    
    // Scene rendered to gFilterSceneTexture
    
    glGenFramebuffers(1, &gFilterSceneFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, gFilterSceneFbo);
    
    glGenTextures(1, &gFilterSceneTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gFilterSceneTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gScreenWidth * 2, gScreenHeight * 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glActiveTexture(GL_TEXTURE0);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gFilterSceneTexture, 0);
    
    glGenRenderbuffers(1, &gFilterSceneRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, gFilterSceneRbo);
    
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, gScreenWidth * 2, gScreenHeight * 2);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gFilterSceneRbo);
    
    GLenum drawModes[1] = {GL_COLOR_ATTACHMENT0};
    
    glDrawBuffers(1, drawModes);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    
    // Results of gaussian filter pass1 stored in gFilterPassTexture (depth buffer not needed)
    
    glGenFramebuffers(1, &gFilterPassFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, gFilterPassFbo);
    
    glGenTextures(1, &gFilterPassTexture);
    glBindTexture(GL_TEXTURE_2D, gFilterPassTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gScreenWidth * 2, gScreenHeight * 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gFilterPassTexture, 0);
    
    GLenum passDrawModes[1] = {GL_COLOR_ATTACHMENT0};
    
    glDrawBuffers(1, passDrawModes);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//OpenGL Loop Logic
void glLoop() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    /*gModelMat = glm::rotate(gModelMat, -0.01f, glm::vec3(0.f, 0.f, 1.f));
    glUniformMatrix4fv(gModelMatLoc, 1, GL_FALSE, glm::value_ptr(gModelMat));*/
    
    glBindFramebuffer(GL_FRAMEBUFFER, gShadowFbo);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(gShadowProg);
    
    glBindVertexArray(gShadowVao);
    
    glViewport(0, 0, SHADOW_DEPTH_TEXTURE_SIZE, SHADOW_DEPTH_TEXTURE_SIZE);
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    glUniformMatrix4fv(gShadowViewMatLoc, 1, GL_FALSE, glm::value_ptr(gShadowViewMat));
    glUniformMatrix4fv(gShadowProjMatLoc, 1, GL_FALSE, glm::value_ptr(gShadowProjMat));
    
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.f, 4.f);
    
    for (auto& e: gSceneGraph) {
        
        glUniformMatrix4fv(gShadowModelMatLoc, 1, GL_FALSE, glm::value_ptr(e.modelMat));
        
        glBindBuffer(GL_ARRAY_BUFFER, e.glPosBuf);
        glVertexAttribPointer(gShadowPosLoc, 4, GL_FLOAT, GL_FALSE, 0, NULL);

        glDrawArrays(GL_TRIANGLES, 0, e.vertCount);
    }
    
    glDisable(GL_POLYGON_OFFSET_FILL);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    
    glViewport(0, 0, gScreenWidth * 2, gScreenHeight * 2);
    
    
    // Renders Scene to gFilterSceneTexture
    
    glBindFramebuffer(GL_FRAMEBUFFER, gFilterSceneFbo);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(gRenderProg);
    
    glBindVertexArray(gVao);
    
    for (auto& e: gSceneGraph) {
        glUniformMatrix4fv(gModelMatLoc, 1, GL_FALSE, glm::value_ptr(e.modelMat));
        glUniform3fv(gAmbientLoc, 1, glm::value_ptr(e.ambientColor));
        glUniform3fv(gDiffuseLoc, 1, glm::value_ptr(e.diffuseColor));
        glUniform3fv(gSpecularLoc, 1, glm::value_ptr(e.specularColor));
        glUniform1f(gShininessLoc, e.shininess);
        glUniform1f(gSpecStrengthLoc, e.specStrength);
        
        glUniform1i(gIsTexturedLoc, e.isTextured);
        glUniform1i(gTexUnitLoc, e.glTexUnit);
        glUniform4fv(gColorLoc, 1, glm::value_ptr(e.objColor));
        
        glBindBuffer(GL_ARRAY_BUFFER, e.glPosBuf);
        glVertexAttribPointer(gPosLoc, 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glBindBuffer(GL_ARRAY_BUFFER, e.glTexCoordBuf);
        glVertexAttribPointer(gTexCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glBindBuffer(GL_ARRAY_BUFFER, e.glNormBuf);
        glVertexAttribPointer(gNormLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        
        glDrawArrays(GL_TRIANGLES, 0, e.vertCount);
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    
    // Pass 1 of gaussian filter (vertical pass). Results stored in gFilterPassTexture
    
    glViewport(0, 0, gScreenWidth * 2, gScreenHeight * 2);
    
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gFilterSceneTexture); // Texture obtained from scene rendering
    glActiveTexture(GL_TEXTURE0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, gFilterPassFbo);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(gFilterProg);
    
    glUniform1i(gFilterPassLoc, 1);
    
    glBindVertexArray(gFilterVao);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    
    // Pass 2 of gaussian filter (horizontal pass). Rendered to default framebuffer (screen)
    
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gFilterPassTexture); // Texture obtained from pass 1 of gaussian filter
    glActiveTexture(GL_TEXTURE0);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(gFilterProg);
    
    glUniform1i(gFilterPassLoc, 2);
    
    glBindVertexArray(gFilterVao);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
}

//OpenGL Shutdown Logic
void glShutdown() {
    glDeleteVertexArrays(1, &gVao);
    
    glDeleteBuffers(1, &gFloorNormBuf);
    glDeleteBuffers(1, &gFloorTexCoordBuf);
    glDeleteBuffers(1, &gFloorPosBuf);
    
    glDeleteBuffers(1, &gPosIndexBuf);
    glDeleteBuffers(1, &gNormBuf);
    glDeleteBuffers(1, &gTexCoordBuf);
    glDeleteBuffers(1, &gPosBuf);
    
    glDeleteTextures(1, &gTexture);
    glDeleteBuffers(1, &gPixelBuf);
    
    glDeleteShader(gRenderFS);
    glDeleteShader(gRenderVS);
    glDeleteProgram(gRenderProg);
    
    stbi_image_free(gTexImage);
}

static void ResizeCallback(GLFWwindow* window, int width, int height) {
    gScreenWidth = width;
    gScreenHeight = height;
    
    GLint projMatLoc = glGetUniformLocation(gRenderProg, "uProjMat");
    glm::mat4 projMat = glm::perspective(45.f, (float)width / (float)height, PROJ_NEAR, PROJ_FAR);
    glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));
}

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main(int argc, const char * argv[]) {
    
    ModelFactory modelFactory;
    teapot = modelFactory.BuildModel("teapot.obj");

    std::cout << teapot.m_Info.vertCount << std::endl;
    
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, NULL, NULL);
    
    gScreenWidth = WIN_WIDTH;
    gScreenHeight = WIN_HEIGHT;
    
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    glInit();
    
    std::cout << glGetString(GL_RENDERER) << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;
    
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetWindowSizeCallback(window, ResizeCallback);
    
    while (!glfwWindowShouldClose(window)) {
        glLoop();
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    glShutdown();
    
    glfwDestroyWindow(window);
    
    glfwTerminate();
    return 0;
}

