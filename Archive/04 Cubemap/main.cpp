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

GLuint gRenderProg;
GLuint gRenderVS;
GLuint gRenderFS;

GLuint gVao;

GLuint gPosBuf;

GLuint gCubemap;

GLint gPosLoc;

GLint gModelMatLoc;
GLint gViewMatLoc;
GLint gProjMatLoc;
GLint gCubemapLoc;

//OpenGL Constant Variables

const GLfloat PROJ_NEAR = 0.1f;
const GLfloat PROJ_FAR = 1000.f;

//Other User-defined Global Variables

glm::mat4 modelMat(1.f);

glm::mat4 gViewMat(1.f);

unsigned char* gTexImage[6];


//OpenGL Initialization Logic
void glInit() {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
    //Program Creation
    
    gRenderProg = glCreateProgram();
    gRenderVS = CreateShader(GL_VERTEX_SHADER, "cubemap.vert");
    gRenderFS = CreateShader(GL_FRAGMENT_SHADER, "cubemap.frag");
    glAttachShader(gRenderProg, gRenderVS);
    glAttachShader(gRenderProg, gRenderFS);
    glLinkProgram(gRenderProg);
    glUseProgram(gRenderProg);
    
    
    //Texture
    
    int imgWidth[6], imgHeight[6], imgComp[6];
    gTexImage[0] = stbi_load("cubemap_posx.jpg", &imgWidth[0], &imgHeight[0], &imgComp[0], STBI_rgb);
    gTexImage[1] = stbi_load("cubemap_negx.jpg", &imgWidth[1], &imgHeight[1], &imgComp[1], STBI_rgb);
    gTexImage[2] = stbi_load("cubemap_posy.jpg", &imgWidth[2], &imgHeight[2], &imgComp[2], STBI_rgb);
    gTexImage[3] = stbi_load("cubemap_negy.jpg", &imgWidth[3], &imgHeight[3], &imgComp[3], STBI_rgb);
    gTexImage[4] = stbi_load("cubemap_posz.jpg", &imgWidth[4], &imgHeight[4], &imgComp[4], STBI_rgb);
    gTexImage[5] = stbi_load("cubemap_negz.jpg", &imgWidth[5], &imgHeight[5], &imgComp[5], STBI_rgb);
    glGenTextures(1, &gCubemap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, gCubemap);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    for (int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, imgWidth[i], imgHeight[i], 0, GL_RGB, GL_UNSIGNED_BYTE, gTexImage[i]);
    }
  
    //Uniforms
    
    gModelMatLoc = glGetUniformLocation(gRenderProg, "uModelMat");
    modelMat = glm::mat4(1.f);
    glUniformMatrix4fv(gModelMatLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
    
    gViewMatLoc = glGetUniformLocation(gRenderProg, "uViewMat");
    gViewMat = glm::rotate(glm::mat4(1.f), -0.5f, glm::vec3(1.f, 0.f, 0.f));
    glUniformMatrix4fv(gViewMatLoc, 1, GL_FALSE, glm::value_ptr(gViewMat));
    
    gProjMatLoc = glGetUniformLocation(gRenderProg, "uProjMat");
    glm::mat4 projMat = glm::perspective(45.f, (float)gScreenWidth / (float)gScreenHeight, PROJ_NEAR, PROJ_FAR);
    glUniformMatrix4fv(gProjMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));
    
    gCubemapLoc = glGetUniformLocation(gRenderProg, "uCubemap");
    glUniform1i(gCubemapLoc, 0);
    
    
    //Vertex Buffer Objects
    
    glm::vec3 cubePos[] = {
        {0.5f, 0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, -0.5f},
        {0.5f, 0.5f, -0.5f}, {0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, -0.5f},
        
        {-0.5f, 0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f}, {-0.5f, 0.5f, 0.5f},
        {-0.5f, 0.5f, 0.5f}, {-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, 0.5f},
        
        {-0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, -0.5f},
        {0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f},
        
        {0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, 0.5f},
        {-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f},
        
        {-0.5f, 0.5f, 0.5f}, {-0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f}, {-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f},
        
        {0.5f, 0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f},
        {-0.5f, 0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f}
    };

    
    glGenBuffers(1, &gPosBuf);
    glBindBuffer(GL_ARRAY_BUFFER, gPosBuf);
    glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(float), &cubePos[0], GL_STREAM_DRAW);


    //Vertex Array Object
    
    glGenVertexArrays(1, &gVao);
    glBindVertexArray(gVao);
    
    
    //Vertex Attribute Specification
    
    gPosLoc = glGetAttribLocation(gRenderProg, "vPosition");
    glEnableVertexAttribArray(gPosLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gPosBuf);
    glVertexAttribPointer(gPosLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    
    //Miscellaneous
}

//OpenGL Loop Logic

void glLoop() {
    gAngleX += gPanX;
    gAngleY += gPanY;
    
    glUseProgram(gRenderProg);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    gViewMat = glm::rotate(glm::mat4(1.f), gAngleY, glm::vec3(1.f, 0.f, 0.f));
    
    gViewMat = glm::rotate(gViewMat, gAngleX, glm::vec3(0.f, 1.f, 0.f));

    glUniformMatrix4fv(gViewMatLoc, 1, GL_FALSE, glm::value_ptr(gViewMat));
    
    glBindVertexArray(gVao);
    
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
}

//OpenGL Shutdown Logic
void glShutdown() {
    glDeleteVertexArrays(1, &gVao);
    
    glDeleteBuffers(1, &gPosBuf);
    
    glDeleteTextures(1, &gCubemap);
    
    glDeleteShader(gRenderFS);
    glDeleteShader(gRenderVS);
    glDeleteProgram(gRenderProg);
    
    for (int i = 0; i < 6; i++) {
        stbi_image_free(gTexImage[i]);
    }
}

static void ResizeCallback(GLFWwindow* window, int width, int height) {
    gScreenWidth = width;
    gScreenHeight = height;
    
    GLint projMatLoc = glGetUniformLocation(gRenderProg, "uProjMat");
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

