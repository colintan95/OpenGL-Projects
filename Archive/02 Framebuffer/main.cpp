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

#include <vector>

#include "Shader.h"
#include "Model.h"

//Configurations

const int GL_VERSION_MAJOR = 3;
const int GL_VERSION_MINOR = 3;

const int WIN_WIDTH = 640;
const int WIN_HEIGHT = 480;

const char* WIN_TITLE = "OpenGL Program";

//GLFW User Variables

int gScreenWidth;
int gScreenHeight;

//OpenGL User-defined Global Variables

GLuint gProgram;
GLuint gVertShader;
GLuint gFragShader;

GLuint gFbo;
GLuint gRbo[2];

GLuint gVao;

GLuint gPosBuf;
GLuint gColorBuf;
GLuint gNormBuf;
GLuint gPosIndexBuf;

GLint gPosLoc;
GLint gColorLoc;
GLint gNormLoc;
GLint gDirLightLoc;


//OpenGL Constant Variables

const GLfloat PROJ_NEAR = 0.1f;
const GLfloat PROJ_FAR = 1000.f;

//Other User-defined Global Variables

Model cube;

glm::vec3 dirLight = {-1.f, 0.8f, 0.5f};

glm::mat4 rotateMat(1.f);

glm::vec3 gCubeColor = {1.f, 0.f, 0.f};
glm::vec3 gCubeHighlight = {1.f, 1.f, 0.f};


//OpenGL Initialization Logic
void glInit() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.f, -1.0f);
    
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
    //World Space Rendering
    
    gProgram = glCreateProgram();
    
    gVertShader = CreateShader(GL_VERTEX_SHADER, "shader.vert");
    gFragShader = CreateShader(GL_FRAGMENT_SHADER, "shader.frag");

    glAttachShader(gProgram, gVertShader);
    glAttachShader(gProgram, gFragShader);
    
    glLinkProgram(gProgram);
    glUseProgram(gProgram);
    
    //Miscellaneous
    
    glGenRenderbuffers(2, gRbo);
    
    glBindRenderbuffer(GL_RENDERBUFFER, gRbo[0]);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, gScreenWidth, gScreenHeight);
    
    glBindRenderbuffer(GL_RENDERBUFFER, gRbo[1]);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, gScreenWidth, gScreenHeight);
    
    glGenFramebuffers(1, &gFbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gFbo);
    
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, gRbo[0]);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gRbo[1]);

    //Uniforms
    
    GLint modelMatLoc = glGetUniformLocation(gProgram, "uModelMat");
    glm::mat4 modelMat = glm::mat4(1.f);
    glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
    
    GLint viewMatLoc = glGetUniformLocation(gProgram, "uViewMat");
    glm::mat4 viewMat = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -20.f));
    glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
    
    GLint projMatLoc = glGetUniformLocation(gProgram, "uProjMat");
    glm::mat4 projMat = glm::perspective(45.f, (float)gScreenWidth / (float)gScreenHeight, PROJ_NEAR, PROJ_FAR);
    glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));
    
    gDirLightLoc = glGetUniformLocation(gProgram, "uDirLight");
    glUniform3fv(gDirLightLoc, 1, &dirLight[0]);
    
    
    //Vertex Buffer Objects
    
    glGenBuffers(1, &gPosBuf);
    glBindBuffer(GL_ARRAY_BUFFER, gPosBuf);
    glBufferData(GL_ARRAY_BUFFER, cube.m_NumVert * 4 * sizeof(float), &cube.m_Info.v[0], GL_STREAM_DRAW);
    
    glGenBuffers(1, &gNormBuf);
    glBindBuffer(GL_ARRAY_BUFFER, gNormBuf);
    glBufferData(GL_ARRAY_BUFFER, cube.m_NumVert * 3 * sizeof(float), &cube.m_Info.vn[0], GL_STREAM_DRAW);
    
    glGenBuffers(1, &gPosIndexBuf);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gPosIndexBuf);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube.m_NumFace * 3 * sizeof(float), &cube.m_Info.fv[0], GL_STATIC_DRAW);
    
    //Vertex Array Object
    
    glGenVertexArrays(1, &gVao);
    glBindVertexArray(gVao);
    
    
    //Vertex Attribute Specification
    
    gPosLoc = glGetAttribLocation(gProgram, "vPosition");
    glEnableVertexAttribArray(gPosLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gPosBuf);
    glVertexAttribPointer(gPosLoc, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    
    gColorLoc = glGetAttribLocation(gProgram, "vColor");
    glDisableVertexAttribArray(gColorLoc);
    glVertexAttrib3fv(gColorLoc, glm::value_ptr(gCubeColor));

    gNormLoc = glGetAttribLocation(gProgram, "vNormal");
    glEnableVertexAttribArray(gNormLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gNormBuf);
    glVertexAttribPointer(gNormLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    
    
    //Miscellaneous
    std::cout << glGetFragDataLocation(gProgram, "outColor") << std::endl;
    
}

//OpenGL Loop Logic
void glLoop() {
    glUseProgram(gProgram);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gFbo);
    
    glViewport(0, 0, gScreenWidth, gScreenHeight);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    cube.Transform(glm::rotate(glm::mat4(1.f), 0.01f, glm::vec3(0.8f, 0.8f, 0.8f)));
    
    glBindBuffer(GL_ARRAY_BUFFER, gPosBuf);
    glBufferSubData(GL_ARRAY_BUFFER, 0, cube.m_NumVert * 4 * sizeof(float), &cube.m_Info.v[0]);
    
    glBindBuffer(GL_ARRAY_BUFFER, gNormBuf);
    glBufferSubData(GL_ARRAY_BUFFER, 0, cube.m_NumVert * 3 * sizeof(float), &cube.m_Info.vn[0]);
    
    glBindVertexArray(gVao);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnableVertexAttribArray(gNormLoc);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gPosIndexBuf);
    glVertexAttrib3fv(gColorLoc, glm::value_ptr(gCubeColor));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDisableVertexAttribArray(gNormLoc);
    glVertexAttrib3fv(gNormLoc, glm::value_ptr(dirLight));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gPosIndexBuf);
    glVertexAttrib3fv(glGetAttribLocation(gProgram, "vColor"), glm::value_ptr(gCubeHighlight));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
    
    //Bind Read and Draw Framebuffer for blitting
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gFbo);
    
    //Binds Draw Framebuffer to default (screen)
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    
    glViewport(0, 0, gScreenWidth, gScreenHeight);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glBlitFramebuffer(0, 0, gScreenWidth, gScreenHeight, 0, 0, gScreenWidth, gScreenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    
    //Renders to screen in reverse x and y axes
    glBlitFramebuffer(0, 0, gScreenWidth, gScreenHeight, gScreenWidth, gScreenHeight, 0, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

//OpenGL Shutdown Logic
void glShutdown() {
    glDeleteVertexArrays(1, &gVao);
    
    glDeleteBuffers(1, &gPosIndexBuf);
    glDeleteBuffers(1, &gNormBuf);
    glDeleteBuffers(1, &gColorBuf);
    glDeleteBuffers(1, &gPosBuf);
    
    glDeleteShader(gFragShader);
    glDeleteShader(gVertShader);
    
    glDeleteProgram(gProgram);
}

static void ResizeCallback(GLFWwindow* window, int width, int height) {
    gScreenWidth = width;
    gScreenHeight = height;
    
    GLint projMatLoc = glGetUniformLocation(gProgram, "uProjMat");
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
    Model model = modelFactory.BuildModel("teapot.obj");
    
    cube = modelFactory.BuildCube(5.f);
    
    //cube.Transform(glm::translate(glm::mat4(1.f), glm::vec3(-10.f, -10.f, -10.f)));
    
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

