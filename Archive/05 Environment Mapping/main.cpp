//
//  main.cpp
//  Template
//
//  Created by Colin Tan on 3/4/15.
//  Copyright (c) 2015 Colin Tan. All rights reserved.
//

#include <iostream>

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

const char* WIN_TITLE = "OpenGL Program";

const float MAX_PAN_SPEED = 0.1f;

//GLFW User Variables

static int gScreenWidth = 640;
static int gScreenHeight = 480;

static float gPanX = 0.f;
static float gPanY = 0.f;

static float gAngleX = 0.f;
static float gAngleY = 0.f;

//OpenGL User-defined Global Variables

GLuint gCubemapProg;
GLuint gCubemapVS;
GLuint gCubemapFS;

GLuint gCubemapVao;

GLuint gCubemapPosBuf;

GLuint gCubemapTexture;

GLint gCubemapPosLoc;

GLint gCubemapModelMatLoc;
GLint gCubemapViewMatLoc;
GLint gCubemapProjMatLoc;
GLint gCubemapTexLoc;


GLuint gRenderProg;
GLuint gRenderVS;
GLuint gRenderFS;

GLuint gRenderVao;

GLuint gRenderPosBuf;
GLuint gRenderNormBuf;

GLint gRenderPosLoc;
GLint gRenderNormLoc;

GLint gRenderModelMatLoc;
GLint gRenderViewMatLoc;
GLint gRenderProjMatLoc;
GLint gRenderLightPosLoc;
GLint gRenderCubemapTexLoc;


//OpenGL Constant Variables

const GLfloat PROJ_NEAR = 0.1f;
const GLfloat PROJ_FAR = 1000.f;

//Other User-defined Global Variables

glm::mat4 modelMat(1.f);

glm::mat4 gViewMat(1.f);
glm::mat4 gProjMat(1.f);

unsigned char* gTexImage[6];

Model gTeapot;


//OpenGL Initialization Logic
void glInit() {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    gViewMat = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -5.f, -35.f));
    gProjMat = glm::perspective(45.f, (float)gScreenWidth / (float)gScreenHeight, PROJ_NEAR, PROJ_FAR);
    
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
    //Program Creation
    
    gCubemapProg = glCreateProgram();
    gCubemapVS = CreateShader(GL_VERTEX_SHADER, "cubemap.vert");
    gCubemapFS = CreateShader(GL_FRAGMENT_SHADER, "cubemap.frag");
    glAttachShader(gCubemapProg, gCubemapVS);
    glAttachShader(gCubemapProg, gCubemapFS);
    glLinkProgram(gCubemapProg);
    glUseProgram(gCubemapProg);
    
    
    //Texture
    
    int imgWidth[6], imgHeight[6], imgComp[6];
    gTexImage[0] = stbi_load("cubemap_posx.jpg", &imgWidth[0], &imgHeight[0], &imgComp[0], STBI_rgb);
    gTexImage[1] = stbi_load("cubemap_negx.jpg", &imgWidth[1], &imgHeight[1], &imgComp[1], STBI_rgb);
    gTexImage[2] = stbi_load("cubemap_posy.jpg", &imgWidth[2], &imgHeight[2], &imgComp[2], STBI_rgb);
    gTexImage[3] = stbi_load("cubemap_negy.jpg", &imgWidth[3], &imgHeight[3], &imgComp[3], STBI_rgb);
    gTexImage[4] = stbi_load("cubemap_posz.jpg", &imgWidth[4], &imgHeight[4], &imgComp[4], STBI_rgb);
    gTexImage[5] = stbi_load("cubemap_negz.jpg", &imgWidth[5], &imgHeight[5], &imgComp[5], STBI_rgb);
    glGenTextures(1, &gCubemapTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, gCubemapTexture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    for (int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, imgWidth[i], imgHeight[i], 0, GL_RGB, GL_UNSIGNED_BYTE, gTexImage[i]);
    }
  
    //Uniforms
    
    gCubemapModelMatLoc = glGetUniformLocation(gCubemapProg, "uModelMat");
    modelMat = glm::mat4(1.f);
    glUniformMatrix4fv(gCubemapModelMatLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
    
    gCubemapViewMatLoc = glGetUniformLocation(gCubemapProg, "uViewMat");
    glUniformMatrix4fv(gCubemapViewMatLoc, 1, GL_FALSE, glm::value_ptr(gViewMat));
    
    gCubemapProjMatLoc = glGetUniformLocation(gCubemapProg, "uProjMat");
    glUniformMatrix4fv(gCubemapProjMatLoc, 1, GL_FALSE, glm::value_ptr(gProjMat));
    
    gCubemapTexLoc = glGetUniformLocation(gCubemapProg, "uCubemap");
    glUniform1i(gCubemapTexLoc, 0);
    
    
    //Vertex Buffer Objects
    
    ModelFactory factory;
    
    Model cube = factory.BuildCube(200.f);
    
    glGenBuffers(1, &gCubemapPosBuf);
    glBindBuffer(GL_ARRAY_BUFFER, gCubemapPosBuf);
    glBufferData(GL_ARRAY_BUFFER, 36 * 4 * sizeof(float), &cube.m_Info.v[0], GL_STREAM_DRAW);


    //Vertex Array Object
    
    glGenVertexArrays(1, &gCubemapVao);
    glBindVertexArray(gCubemapVao);
    
    
    //Vertex Attribute Specification
    
    gCubemapPosLoc = glGetAttribLocation(gCubemapProg, "vPosition");
    glEnableVertexAttribArray(gCubemapPosLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gCubemapPosBuf);
    glVertexAttribPointer(gCubemapPosLoc, 4, GL_FLOAT, GL_FALSE, 0, NULL);

    
    
    gTeapot = factory.BuildModel("teapot.obj");
    
    gRenderProg = glCreateProgram();
    gRenderVS = CreateShader(GL_VERTEX_SHADER, "env_map_render.vert");
    gRenderFS = CreateShader(GL_FRAGMENT_SHADER, "env_map_render.frag");
    glAttachShader(gRenderProg, gRenderVS);
    glAttachShader(gRenderProg, gRenderFS);
    glLinkProgram(gRenderProg);
    glUseProgram(gRenderProg);
    
    gRenderModelMatLoc = glGetUniformLocation(gRenderProg, "uModelMat");
    glm::mat4 renderModelMat = glm::rotate(glm::mat4(1.f), -1.f, glm::vec3(1.f, 0.f, 0.f));
    //glm::mat4 renderModelMat = glm::mat4(1.f);
    glUniformMatrix4fv(gRenderModelMatLoc, 1, GL_FALSE, glm::value_ptr(renderModelMat));
    
    gRenderViewMatLoc = glGetUniformLocation(gRenderProg, "uViewMat");
    glUniformMatrix4fv(gRenderViewMatLoc, 1, GL_FALSE, glm::value_ptr(gViewMat));
    
    gRenderProjMatLoc = glGetUniformLocation(gRenderProg, "uProjMat");
    glUniformMatrix4fv(gRenderProjMatLoc, 1, GL_FALSE, glm::value_ptr(gProjMat));

    gRenderCubemapTexLoc = glGetUniformLocation(gRenderProg, "uCubemapTex");
    glUniform1i(gRenderCubemapTexLoc, 0);
    
    glGenBuffers(1, &gRenderPosBuf);
    glBindBuffer(GL_ARRAY_BUFFER, gRenderPosBuf);
    glBufferData(GL_ARRAY_BUFFER, gTeapot.m_Info.vertCount * 4 * sizeof(float), &gTeapot.m_Info.v[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &gRenderNormBuf);
    glBindBuffer(GL_ARRAY_BUFFER, gRenderNormBuf);
    glBufferData(GL_ARRAY_BUFFER, gTeapot.m_Info.vertCount * 3 * sizeof(float), &gTeapot.m_Info.vn[0], GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &gRenderVao);
    glBindVertexArray(gRenderVao);
    
    gRenderPosLoc = glGetAttribLocation(gRenderProg, "vPosition");
    glEnableVertexAttribArray(gRenderPosLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gRenderPosBuf);
    glVertexAttribPointer(gRenderPosLoc, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    
    gRenderNormLoc = glGetAttribLocation(gRenderProg, "vNormal");
    glEnableVertexAttribArray(gRenderNormLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gRenderNormBuf);
    glVertexAttribPointer(gRenderNormLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    //Miscellaneous
}

//OpenGL Loop Logic

void glLoop() {
    gAngleX += gPanX;
    gAngleY += gPanY;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(gCubemapProg);
    //gViewMat = glm::rotate(gViewMat, gAngleY, glm::vec3(1.f, 0.f, 0.f));
    //gViewMat = glm::rotate(gViewMat, gAngleX, glm::vec3(0.f, 1.f, 0.f));
    //glUniformMatrix4fv(gCubemapViewMatLoc, 1, GL_FALSE, glm::value_ptr(gViewMat));
    glBindVertexArray(gCubemapVao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    
    glUseProgram(gRenderProg);
    glBindVertexArray(gRenderVao);
    glDrawArrays(GL_TRIANGLES, 0, gTeapot.m_Info.vertCount);
}

//OpenGL Shutdown Logic
void glShutdown() {
    
    glDeleteVertexArrays(1, &gRenderVao);
    glDeleteBuffers(1, &gRenderNormBuf);
    glDeleteBuffers(1, &gRenderPosBuf);
    glDeleteShader(gRenderFS);
    glDeleteShader(gRenderVS);
    glDeleteProgram(gRenderProg);
    
    
    glDeleteVertexArrays(1, &gCubemapVao);
    glDeleteBuffers(1, &gCubemapPosBuf);
    glDeleteTextures(1, &gCubemapTexture);
    glDeleteShader(gCubemapFS);
    glDeleteShader(gCubemapVS);
    glDeleteProgram(gCubemapProg);
    
    for (int i = 0; i < 6; i++) {
        stbi_image_free(gTexImage[i]);
    }
}

static void ResizeCallback(GLFWwindow* window, int width, int height) {
    gScreenWidth = width;
    gScreenHeight = height;
    
    GLint projMatLoc = glGetUniformLocation(gCubemapProg, "uProjMat");
    glm::mat4 projMat = glm::perspective(45.f, (float)width / (float)height, PROJ_NEAR, PROJ_FAR);
    glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));
}

static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    double midx = gScreenWidth / 2;
    double midy = gScreenHeight / 2;
    
    if (xpos > 0 && xpos < gScreenWidth) {
        gPanX = MAX_PAN_SPEED * ((xpos - midx) / (gScreenWidth / 2));
    }
    
    if (ypos > 0 && ypos < gScreenHeight) {
        gPanY = MAX_PAN_SPEED * ((ypos - midy) / (gScreenHeight / 2));
    }
}

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main(int argc, const char * argv[]) {
    
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(gScreenWidth, gScreenHeight, WIN_TITLE, NULL, NULL);
    
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
    glfwSetCursorPosCallback(window, CursorPosCallback);
    
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

