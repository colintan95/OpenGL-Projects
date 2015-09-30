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

//GLFW User Variables

static int gScreenWidth = 640;
static int gScreenHeight = 480;


//OpenGL User-defined Global Variables

GLuint gRenderProg;
GLuint gRenderVS;
GLuint gRenderFS;

GLuint gRenderVao;

GLuint gRenderPosBuf;

GLuint gRenderTexture;

GLint gRenderPosLoc;

GLint gRenderModelMatLoc;
GLint gRenderViewMatLoc;
GLint gRenderProjMatLoc;
GLint gRenderTextureLoc;

//OpenGL Constant Variables

const GLfloat PROJ_NEAR = 0.1f;
const GLfloat PROJ_FAR = 1000.f;

//Other User-defined Global Variables

glm::mat4 modelMat(1.f);

glm::mat4 gViewMat(1.f);
glm::mat4 gProjMat(1.f);

unsigned char* gTexImage;


//OpenGL Initialization Logic
void glInit() {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    gViewMat = glm::mat4(1.f);
    gProjMat = glm::perspective(45.f, (float)gScreenWidth / (float)gScreenHeight, PROJ_NEAR, PROJ_FAR);
    
    glEnable(GL_PROGRAM_POINT_SIZE);

    gRenderProg = glCreateProgram();
    gRenderVS = CreateShader(GL_VERTEX_SHADER, "point_sprite.vert");
    gRenderFS = CreateShader(GL_FRAGMENT_SHADER, "point_sprite.frag");
    glAttachShader(gRenderProg, gRenderVS);
    glAttachShader(gRenderProg, gRenderFS);
    glLinkProgram(gRenderProg);
    glUseProgram(gRenderProg);
    
    int imgWidth, imgHeight, imgComp;
    gTexImage = stbi_load("star.png", &imgWidth, &imgHeight, &imgComp, STBI_rgb_alpha);
    glGenTextures(1, &gRenderTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gRenderTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, gTexImage);
    
    std::cout << imgWidth << std::endl;
    
    gRenderModelMatLoc = glGetUniformLocation(gRenderProg, "uModelMat");
    glm::mat4 modelMat = glm::mat4(1.f);
    glUniformMatrix4fv(gRenderModelMatLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
    
    gRenderViewMatLoc = glGetUniformLocation(gRenderProg, "uViewMat");
    glUniformMatrix4fv(gRenderViewMatLoc, 1, GL_FALSE, glm::value_ptr(gViewMat));
    
    gRenderProjMatLoc = glGetUniformLocation(gRenderProg, "uProjMat");
    glUniformMatrix4fv(gRenderProjMatLoc, 1, GL_FALSE, glm::value_ptr(gProjMat));
    
    gRenderTextureLoc = glGetUniformLocation(gRenderProg, "uTexture");
    glUniform1i(gRenderTextureLoc, 0);
    
    glGenBuffers(1, &gRenderPosBuf);
    glBindBuffer(GL_ARRAY_BUFFER, gRenderPosBuf);
    glm::vec4 renderPosData[] = {
        {-1.f, 1.f, -5.f, 1.f}, {1.f, 1.f, -10.f, 1.f}, {1.f, -1.f, -15.f, 1.f}, {-1.f, -1.f, -10.f, 1.f}
    };
    /*glm::vec4 renderPosData[] = {
        {0.f, 2.f, -10.f, 1.f}, {0.f, 0.f, -10.f, 1.f}, {2.f, 2.f, -10.f, 1.f},
        {2.f, 2.f, -10.f, 1.f}, {0.f, 0.f, -10.f, 1.f}, {2.f, 0.f, -10.f, 1.f},
        
        {-3.f, 3.f, -10.f, 1.f}, {-3.f, 1.f, -10.f, 1.f}, {-1.f, 3.f, -10.f, 1.f},
        {-1.f, 3.f, -10.f, 1.f}, {-3.f, 1.f, -10.f, 1.f}, {-1.f, 1.f, -10.f, 1.f}
    };*/
    glBufferData(GL_ARRAY_BUFFER, 4 * 4 * sizeof(float), &renderPosData[0], GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &gRenderVao);
    glBindVertexArray(gRenderVao);
    
    gRenderPosLoc = glGetAttribLocation(gRenderProg, "vPosition");
    glEnableVertexAttribArray(gRenderPosLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gRenderPosBuf);
    glVertexAttribPointer(gRenderPosLoc, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    
    //Miscellaneous
}

//OpenGL Loop Logic

void glLoop() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(gRenderProg);
    glBindVertexArray(gRenderVao);
    glDrawArrays(GL_POINTS, 0, 4);
    //glDrawArrays(GL_TRIANGLES, 0, 12);
}

//OpenGL Shutdown Logic
void glShutdown() {

    glDeleteVertexArrays(1, &gRenderVao);
    glDeleteBuffers(1, &gRenderPosBuf);
    glDeleteTextures(1, &gRenderTexture);
    glDeleteShader(gRenderFS);
    glDeleteShader(gRenderVS);
    glDeleteProgram(gRenderProg);
    
    stbi_image_free(gTexImage);
}

static void ResizeCallback(GLFWwindow* window, int width, int height) {
    gScreenWidth = width;
    gScreenHeight = height;
    
    gProjMat = glm::perspective(45.f, (float)width / (float)height, PROJ_NEAR, PROJ_FAR);
    glUniformMatrix4fv(gRenderProjMatLoc, 1, GL_FALSE, glm::value_ptr(gProjMat));
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

