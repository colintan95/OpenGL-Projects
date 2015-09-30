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

//OpenGL User-defined Global Variables


GLuint gScreenProgram;
GLuint gScreenVertShader;
GLuint gScreenFragShader;

GLuint gScreenVao;

GLuint gScreenPosBuf;
GLuint gScreenColorBuf;


GLuint gProgram;
GLuint gVertShader;
GLuint gFragShader;

GLint gDirLightLoc;

GLuint gPosBuf;
GLuint gColorBuf;
GLuint gNormBuf;
GLuint gModelMatBuf;

GLuint gColorTbo;
GLuint gModelMatTbo;

GLuint gPosIndexBuf;

GLuint gVao;

//Other User-defined Global Variables

Model cube;

glm::mat4 gMvpMat(1.f);

glm::vec4 colorPts[] = {
    {1.0f, 0.0f, 0.0f, 1.0f},
};

glm::mat4 gModelMatData[64];

glm::vec4 gColorData[64];

glm::vec3 dirLight = {-1.f, 0.8f, 0.5f};

glm::mat4 rotateMat(1.f);

//OpenGL Initialization Logic
void glInit() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //Screen Rendering
    
    gScreenProgram = glCreateProgram();
    
    gScreenVertShader = CreateShader(GL_VERTEX_SHADER, "screen.vert");
    gScreenFragShader = CreateShader(GL_FRAGMENT_SHADER, "screen.frag");
    
    glAttachShader(gScreenProgram, gScreenVertShader);
    glAttachShader(gScreenProgram, gScreenFragShader);
    
    glLinkProgram(gScreenProgram);
    glUseProgram(gScreenProgram);
    
    glm::vec4 screenPosPts[] = {
        {-0.8f, -0.8f, 0.f, 1.f},
        {0.f, 0.8f, 0.f, 1.f},
        {0.8f, -0.8f, 0.f, 1.f}
    };
    
    glm::vec4 screenColorPts[] = {
        {1.f, 0.f, 0.f, 1.f},
        {1.f, 0.f, 0.f, 1.f},
        {1.f, 0.f, 0.f, 1.f}
    };
    
    glGenBuffers(1, &gScreenPosBuf);
    glBindBuffer(GL_ARRAY_BUFFER, gScreenPosBuf);
    glBufferData(GL_ARRAY_BUFFER, 3 * 4 * sizeof(float), screenPosPts, GL_STATIC_DRAW);
    
    glGenBuffers(1, &gScreenColorBuf);
    glBindBuffer(GL_ARRAY_BUFFER, gScreenColorBuf);
    glBufferData(GL_ARRAY_BUFFER, 3 * 4 * sizeof(float), screenColorPts, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &gScreenVao);
    glBindVertexArray(gScreenVao);
    
    GLint screenPosLoc = glGetAttribLocation(gScreenProgram, "vPosition");
    glEnableVertexAttribArray(screenPosLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gScreenPosBuf);
    glVertexAttribPointer(screenPosLoc, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    
    GLint screenColorLoc = glGetAttribLocation(gScreenProgram, "vColor");
    glEnableVertexAttribArray(screenColorLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gScreenColorBuf);
    glVertexAttribPointer(screenColorLoc, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    
    glEnable(GL_STENCIL_TEST);
    glDepthMask(GL_FALSE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glClear(GL_STENCIL_BUFFER_BIT);
    glStencilFunc(GL_ALWAYS, 1, 1);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    
    glBindVertexArray(gScreenVao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    
    
    //World Space Rendering
    
    gProgram = glCreateProgram();
    
    gVertShader = CreateShader(GL_VERTEX_SHADER, "shader.vert");
    gFragShader = CreateShader(GL_FRAGMENT_SHADER, "shader.frag");

    glAttachShader(gProgram, gVertShader);
    glAttachShader(gProgram, gFragShader);
    
    glLinkProgram(gProgram);
    glUseProgram(gProgram);
    
    //Miscellaneous
    
    for (int i = 0; i < 64; i++) {
        float dx = (float)((i % 2) * 2 - 1) * 10.f;
        float dy = (float)((((i % 4) / 2) >= 1) * 2 - 1) * 10.f;
        float dz = i * -2.f;
        
        gModelMatData[i] = glm::translate(glm::mat4(1.f), glm::vec3(dx, dy, dz));
        
        float ci = i % 3;
        float cr = (ci == 0) * 1.0f;
        float cg = (ci == 1) * 1.0f;
        float cb = (ci == 2) * 1.0f;
        
        gColorData[i] = glm::vec4(cr, cg, cb, 1.f);
    }
    
    
    //Uniforms
    
    GLint colorLoc = glGetUniformLocation(gProgram, "colorTbo");
    glUniform1i(colorLoc, 0);
    
    GLint modelMatLoc = glGetUniformLocation(gProgram, "modelMatTbo");
    glUniform1i(modelMatLoc, 1);
    
    GLint viewMatLoc = glGetUniformLocation(gProgram, "viewMat");
    glm::mat4 viewMat = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -30.f));
    glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
    
    GLint projMatLoc = glGetUniformLocation(gProgram, "projMat");
    glm::mat4 projMat = glm::perspective(45.f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.f);
    glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));
    
    gDirLightLoc = glGetUniformLocation(gProgram, "dirLight");
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
    
    glGenBuffers(1, &gModelMatBuf);
    glBindBuffer(GL_TEXTURE_BUFFER, gModelMatBuf);
    glBufferData(GL_TEXTURE_BUFFER, 64 * 16 * sizeof(float), gModelMatData, GL_STATIC_DRAW);
    
    glGenBuffers(1, &gColorBuf);
    glBindBuffer(GL_TEXTURE_BUFFER, gColorBuf);
    glBufferData(GL_TEXTURE_BUFFER, 64 * 4 * sizeof(float), gColorData, GL_STATIC_DRAW);
    
    
    //Texture Buffer Objects
    
    glGenTextures(1, &gColorTbo);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_BUFFER, gColorTbo);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, gColorBuf);
    
    glGenTextures(1, &gModelMatTbo);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_BUFFER, gModelMatTbo);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, gModelMatBuf);
    
    
    //Vertex Array Object
    
    glGenVertexArrays(1, &gVao);
    glBindVertexArray(gVao);
    
    
    //Vertex Attribute Specification
    
    GLint posLoc = glGetAttribLocation(gProgram, "vPosition");
    glEnableVertexAttribArray(posLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gPosBuf);
    glVertexAttribPointer(posLoc, 4, GL_FLOAT, GL_FALSE, 0, NULL);

    GLint normLoc = glGetAttribLocation(gProgram, "vNormal");
    glEnableVertexAttribArray(normLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gNormBuf);
    glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    
    //Miscellaneous
}

//OpenGL Loop Logic
void glLoop() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glStencilFunc(GL_EQUAL, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    
    /*cube.Transform(glm::rotate(glm::mat4(1.f), 0.01f, glm::vec3(0.8f, 0.8f, 0.8f)));
    
    glBindBuffer(GL_ARRAY_BUFFER, gPosBuf);
    glBufferSubData(GL_ARRAY_BUFFER, 0, cube.m_NumVert * 4 * sizeof(float), &cube.m_Info.v[0]);
    
    glBindBuffer(GL_ARRAY_BUFFER, gNormBuf);
    glBufferSubData(GL_ARRAY_BUFFER, 0, cube.m_NumVert * 3 * sizeof(float), &cube.m_Info.vn[0]);*/
    
    glUseProgram(gProgram);
    
    glBindVertexArray(gVao);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gPosIndexBuf);
    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
    glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr, 64);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gPosIndexBuf);
    glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr, 64);
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

