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
#include <chrono>

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

GLuint gParticleProg;
GLuint gParticleVS;
GLuint gParticleFS;

GLuint gTfo[2];

GLuint gParticleVao[2];

GLint gPositionLoc;
GLint gVelocityLoc;
GLint gStartTimeLoc;
GLint gInitVelocityLoc;

GLuint gPositionBuf[2];
GLuint gVelocityBuf[2];
GLuint gStartTimeBuf[2];
GLuint gInitVelocityBuf[2];

GLint gTimeLoc;
GLint gFrametimeLoc;
GLint gLifetimeLoc;
GLint gAccelerationLoc;
GLint gMinParticleSizeLoc;
GLint gMaxParticleSizeLoc;
GLint gModelMatLoc;
GLint gViewMatLoc;
GLint gProjMatLoc;
GLint gTextureLoc;
GLint gPassLoc;

GLuint gParticleTexture;

//OpenGL Constant Variables

const GLfloat PROJ_NEAR = 0.1f;
const GLfloat PROJ_FAR = 1000.f;

//Other User-defined Global Variables

const int kNumParticles = 100;

glm::mat4 gModelMat(1.f);

auto gStartTime = std::chrono::high_resolution_clock::now();
auto gPrevFrameTime = std::chrono::high_resolution_clock::now();
int gDrawBuf = 1;

unsigned char* gParticleImage;

float randFloat() {
    return (float)rand() / (float)RAND_MAX;
}

//OpenGL Initialization Logic
void glInit() {
    glEnable(GL_TEXTURE_2D);
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.f, -1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(10.f);
    
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
    int imgWidth, imgHeight, imgComp;
    gParticleImage = stbi_load("circle.png", &imgWidth, &imgHeight, &imgComp, STBI_rgb_alpha);
    glGenTextures(1, &gParticleTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gParticleTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, gParticleImage);
    glActiveTexture(GL_TEXTURE0);
    
    gParticleProg = glCreateProgram();
    gParticleVS = CreateShader(GL_VERTEX_SHADER, "particle.vert");
    gParticleFS = CreateShader(GL_FRAGMENT_SHADER, "particle.frag");
    glAttachShader(gParticleProg, gParticleVS);
    glAttachShader(gParticleProg, gParticleFS);
    
    glGenTransformFeedbacks(2, gTfo);
    const char* tfoVaryings[] = { "trPosition", "trVelocity", "trStartTime" };
    
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, gTfo[0]);
    glTransformFeedbackVaryings(gParticleProg, 3, tfoVaryings, GL_SEPARATE_ATTRIBS);
    
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, gTfo[1]);
    glTransformFeedbackVaryings(gParticleProg, 3, tfoVaryings, GL_SEPARATE_ATTRIBS);
    
    glLinkProgram(gParticleProg);
    glUseProgram(gParticleProg);
    
    gTimeLoc = glGetUniformLocation(gParticleProg, "uTime");
    glUniform1f(gTimeLoc, 0.f);
    
    gFrametimeLoc = glGetUniformLocation(gParticleProg, "uFrametime");
    glUniform1f(gFrametimeLoc, 0.f);
    
    gLifetimeLoc = glGetUniformLocation(gParticleProg, "uLifetime");
    glUniform1f(gLifetimeLoc, 5.f);
    
    gAccelerationLoc = glGetUniformLocation(gParticleProg, "uAcceleration");
    glm::vec3 acceleration = {0.f, -0.1f, 0.f};
    glUniform3fv(gAccelerationLoc, 1, glm::value_ptr(acceleration));
    
    gMinParticleSizeLoc = glGetUniformLocation(gParticleProg, "uMinParticleSize");
    glUniform1f(gMinParticleSizeLoc, 5.f);
    
    gMaxParticleSizeLoc = glGetUniformLocation(gParticleProg, "uMaxParticleSize");
    glUniform1f(gMaxParticleSizeLoc, 50.f);
    
    
    gModelMatLoc = glGetUniformLocation(gParticleProg, "uModelMat");
    gModelMat = glm::mat4(1.f);
    //gModelMat = glm::rotate(glm::mat4(1.f), -1.57f, glm::vec3(1.f, 0.f, 0.f));
    glUniformMatrix4fv(gModelMatLoc, 1, GL_FALSE, glm::value_ptr(gModelMat));
    
    gViewMatLoc = glGetUniformLocation(gParticleProg, "uViewMat");
    glm::mat4 viewMat = glm::mat4(1.f);
    //viewMat = glm::rotate(glm::mat4(1.f), 0.7f, glm::vec3(1.f, 0.f, 0.f)) * viewMat;
    viewMat = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -10.f)) * viewMat;
    glUniformMatrix4fv(gViewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
    
    gProjMatLoc = glGetUniformLocation(gParticleProg, "uProjMat");
    glm::mat4 projMat = glm::perspective(45.f, (float)gScreenWidth / (float)gScreenHeight, PROJ_NEAR, PROJ_FAR);
    glUniformMatrix4fv(gProjMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));
    
    gTextureLoc = glGetUniformLocation(gParticleProg, "uTexture");
    glUniform1i(gTextureLoc, 1);
    
    gPassLoc = glGetUniformLocation(gParticleProg, "uPass");
    glUniform1i(gPassLoc, 1);
    
    float* posData = new float[kNumParticles * 3];
    float* vltyData = new float[kNumParticles * 3];
    float* timeData = new float[kNumParticles];
    for (int i = 0; i < kNumParticles; i++) {
        // Generate random velocities using spherical coordinates
        float theta = glm::mix(0.f, (float)M_PI / 6.f, randFloat());
        float phi = glm::mix(0.f, (float)M_PI * 2.f, randFloat());
        glm::vec3 dir = { sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi)};
        float mag = glm::mix(0.5f, 1.0f, randFloat()); // Assigns a random magnitude to the velocity
        dir *= mag;
        
        vltyData[i * 3 + 0] = dir.x;
        vltyData[i * 3 + 1] = dir.y;
        vltyData[i * 3 + 2] = dir.z;
        
        timeData[i] = 1.f + i * 0.05f; // Simulation starts after a 1 second delay
        posData[i * 3 + 0] = 0.f;
        posData[i * 3 + 1] = 0.f;
        posData[i * 3 + 2] = 0.f;
    }
    
    glGenBuffers(2, gPositionBuf);
    glBindBuffer(GL_ARRAY_BUFFER, gPositionBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, kNumParticles * 3 * sizeof(float), posData, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, gPositionBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, kNumParticles * 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
    
    glGenBuffers(2, gVelocityBuf);
    glBindBuffer(GL_ARRAY_BUFFER, gVelocityBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, kNumParticles * 3 * sizeof(float), vltyData, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, gVelocityBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, kNumParticles * 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
    
    glGenBuffers(2, gStartTimeBuf);
    glBindBuffer(GL_ARRAY_BUFFER, gStartTimeBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, kNumParticles * sizeof(float), timeData, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, gStartTimeBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, kNumParticles * sizeof(float), NULL, GL_DYNAMIC_DRAW);
    
    glGenBuffers(2, gInitVelocityBuf);
    glBindBuffer(GL_ARRAY_BUFFER, gInitVelocityBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, kNumParticles * 3 * sizeof(float), vltyData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, gInitVelocityBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, kNumParticles * 3 * sizeof(float), vltyData, GL_STATIC_DRAW);
    
    delete[] timeData;
    delete[] vltyData;
    delete[] posData;
    

    gPositionLoc = glGetAttribLocation(gParticleProg, "vPosition");
    gVelocityLoc = glGetAttribLocation(gParticleProg, "vVelocity");
    gStartTimeLoc = glGetAttribLocation(gParticleProg, "vStartTime");
    gInitVelocityLoc = glGetAttribLocation(gParticleProg, "vInitVelocity");
    
    glGenVertexArrays(2, gParticleVao);
    
    glBindVertexArray(gParticleVao[0]);
    
    glEnableVertexAttribArray(gPositionLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gPositionBuf[0]);
    glVertexAttribPointer(gPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray(gVelocityLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gVelocityBuf[0]);
    glVertexAttribPointer(gVelocityLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    glEnableVertexAttribArray(gStartTimeLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gStartTimeBuf[0]);
    glVertexAttribPointer(gStartTimeLoc, 1, GL_FLOAT, GL_FALSE, 0, NULL);
    
    glEnableVertexAttribArray(gInitVelocityLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gInitVelocityBuf[0]);
    glVertexAttribPointer(gInitVelocityLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    glBindVertexArray(gParticleVao[1]);
    
    glEnableVertexAttribArray(gPositionLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gPositionBuf[1]);
    glVertexAttribPointer(gPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    glEnableVertexAttribArray(gVelocityLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gVelocityBuf[1]);
    glVertexAttribPointer(gVelocityLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    glEnableVertexAttribArray(gStartTimeLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gStartTimeBuf[1]);
    glVertexAttribPointer(gStartTimeLoc, 1, GL_FLOAT, GL_FALSE, 0, NULL);
    
    glEnableVertexAttribArray(gInitVelocityLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gInitVelocityBuf[1]);
    glVertexAttribPointer(gInitVelocityLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, gTfo[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, gPositionBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, gVelocityBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, gStartTimeBuf[0]);
    
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, gTfo[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, gPositionBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, gVelocityBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, gStartTimeBuf[1]);
}

//OpenGL Loop Logic
void glLoop() {
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    float appTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - gStartTime).count() / 1000000.f;
    float frameTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - gPrevFrameTime).count() / 1000000.f;
    gPrevFrameTime = currentTime;
    
    // Update pass
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glEnable(GL_RASTERIZER_DISCARD); // Disables rendering
    
    glUseProgram(gParticleProg);
    
    glUniform1f(gTimeLoc, appTime);
    glUniform1f(gFrametimeLoc, frameTime);
    glUniform1i(gPassLoc, 1);
    
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, gTfo[gDrawBuf]);
    
    glBeginTransformFeedback(GL_POINTS);
    
    glBindVertexArray(gParticleVao[1 - gDrawBuf]); // Use the non-drawing buffer as input
    
    glDrawArrays(GL_POINTS, 0, kNumParticles);
    
    glEndTransformFeedback();
    
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    
    glDisable(GL_RASTERIZER_DISCARD);
    
    
    // Render pass
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(gParticleProg);
    
    glUniform1i(gPassLoc, 2);
    
    glBindVertexArray(gParticleVao[gDrawBuf]); // Uses attributes from transform feedback
    
    glDrawArrays(GL_POINTS, 0, kNumParticles);
    
    
    gDrawBuf = 1 - gDrawBuf; // Swap draw buffer
}

//OpenGL Shutdown Logic
void glShutdown() {
    glDeleteVertexArrays(2, gParticleVao);
    
    glDeleteBuffers(2, gInitVelocityBuf);
    glDeleteBuffers(2, gStartTimeBuf);
    glDeleteBuffers(2, gVelocityBuf);
    glDeleteBuffers(2, gPositionBuf);
    
    glDeleteTextures(1, &gParticleTexture);
    
    glDeleteTransformFeedbacks(2, gTfo);
    
    glDeleteShader(gParticleFS);
    glDeleteShader(gParticleVS);
    glDeleteProgram(gParticleProg);
    
    stbi_image_free(gParticleImage);
}

static void ResizeCallback(GLFWwindow* window, int width, int height) {
    gScreenWidth = width;
    gScreenHeight = height;
    
    /*GLint projMatLoc = glGetUniformLocation(gParticleProg, "uProjMat");
    glm::mat4 projMat = glm::perspective(45.f, (float)width / (float)height, PROJ_NEAR, PROJ_FAR);
    glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));*/
}

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main(int argc, const char * argv[]) {
    srand(time(0));
    
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);
    
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

