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

GLuint gPixelBuf;
GLuint gTexture;

GLint gPosLoc;
GLint gTexCoordLoc;
GLint gNormLoc;

GLint gModelMatLoc;
GLint gViewMatLoc;
GLint gProjMatLoc;
GLint gTexSamplerLoc;

GLint gAmbientLoc;
GLint gDiffuseLoc;
GLint gSpecularLoc;
GLint gShininessLoc;
GLint gLightPosLoc;

//OpenGL Constant Variables

const GLfloat PROJ_NEAR = 0.1f;
const GLfloat PROJ_FAR = 1000.f;

//Other User-defined Global Variables

Model teapot;

glm::mat4 modelMat(1.f);

unsigned char* gTexImage;


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
    
    //Program Creation
    
    gRenderProg = glCreateProgram();
    gRenderVS = CreateShader(GL_VERTEX_SHADER, "shader.vert");
    gRenderFS = CreateShader(GL_FRAGMENT_SHADER, "shader.frag");
    glAttachShader(gRenderProg, gRenderVS);
    glAttachShader(gRenderProg, gRenderFS);
    glLinkProgram(gRenderProg);
    glUseProgram(gRenderProg);
    
    
    //Texture
    
    int imgWidth, imgHeight, imgComp;
    gTexImage = stbi_load("texture.jpg", &imgWidth, &imgHeight, &imgComp, STBI_rgb);
    glGenBuffers(1, &gPixelBuf);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, gPixelBuf);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, imgWidth * imgHeight * imgComp * sizeof(unsigned char), gTexImage, GL_STATIC_DRAW);
    glGenTextures(1, &gTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  
    //Uniforms
    
    gModelMatLoc = glGetUniformLocation(gRenderProg, "uModelMat");
    modelMat = glm::rotate(glm::mat4(1.f), -0.8f, glm::vec3(1.f, 0.f, 0.f));
    glUniformMatrix4fv(gModelMatLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
    
    gViewMatLoc = glGetUniformLocation(gRenderProg, "uViewMat");
    glm::mat4 viewMat = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -50.f));
    glUniformMatrix4fv(gViewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
    
    gProjMatLoc = glGetUniformLocation(gRenderProg, "uProjMat");
    glm::mat4 projMat = glm::perspective(45.f, (float)gScreenWidth / (float)gScreenHeight, PROJ_NEAR, PROJ_FAR);
    glUniformMatrix4fv(gProjMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));
    
    gTexSamplerLoc = glGetUniformLocation(gRenderProg, "uTexSampler");
    glUniform1i(gTexSamplerLoc, 0);
    
    gAmbientLoc = glGetUniformLocation(gRenderProg, "uAmbient");
    glm::vec4 ambVec = {0.3f, 0.3f, 0.3f, 1.f};
    glUniform4fv(gAmbientLoc, 1, glm::value_ptr(ambVec));
    
    gDiffuseLoc = glGetUniformLocation(gRenderProg, "uDiffuse");
    glm::vec4 difVec = {0.5f, 0.f, 0.f, 1.f};
    glUniform4fv(gDiffuseLoc, 1, glm::value_ptr(difVec));
    
    gSpecularLoc = glGetUniformLocation(gRenderProg, "uSpecular");
    glm::vec4 specVec = {1.f, 1.f, 1.f, 1.f};
    glUniform4fv(gSpecularLoc, 1, glm::value_ptr(specVec));
    
    gShininessLoc = glGetUniformLocation(gRenderProg, "uShininess");
    float shininess = 16.0f;
    glUniform1f(gShininessLoc, shininess);
    
    gLightPosLoc = glGetUniformLocation(gRenderProg, "uLightPos");
    glm::vec4 lightPos = {30.f, 10.f, 30.f, 1.f};
    glUniform4fv(gLightPosLoc, 1, glm::value_ptr(lightPos));
    
    
    //Vertex Buffer Objects
    
    glGenBuffers(1, &gPosBuf);
    glBindBuffer(GL_ARRAY_BUFFER, gPosBuf);
    glBufferData(GL_ARRAY_BUFFER, teapot.m_Info.vertCount * 4 * sizeof(float), &teapot.m_Info.v[0], GL_STREAM_DRAW);
    
    glGenBuffers(1, &gTexCoordBuf);
    glBindBuffer(GL_ARRAY_BUFFER, gTexCoordBuf);
    glBufferData(GL_ARRAY_BUFFER, teapot.m_Info.vertCount * 2 * sizeof(float), &teapot.m_Info.vt[0], GL_STREAM_DRAW);
    
    glGenBuffers(1, &gNormBuf);
    glBindBuffer(GL_ARRAY_BUFFER, gNormBuf);
    glBufferData(GL_ARRAY_BUFFER, teapot.m_Info.vertCount * 3 * sizeof(float), &teapot.m_Info.vn[0], GL_STREAM_DRAW);

    //Vertex Array Object
    
    glGenVertexArrays(1, &gVao);
    glBindVertexArray(gVao);
    
    
    //Vertex Attribute Specification
    
    gPosLoc = glGetAttribLocation(gRenderProg, "vPosition");
    glEnableVertexAttribArray(gPosLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gPosBuf);
    glVertexAttribPointer(gPosLoc, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    
    gTexCoordLoc = glGetAttribLocation(gRenderProg, "vTexCoord");
    glEnableVertexAttribArray(gTexCoordLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gTexCoordBuf);
    glVertexAttribPointer(gTexCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    gNormLoc = glGetAttribLocation(gRenderProg, "vNormal");
    glEnableVertexAttribArray(gNormLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gNormBuf);
    glVertexAttribPointer(gNormLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    
    //Miscellaneous
}

//OpenGL Loop Logic
void glLoop() {
    glUseProgram(gRenderProg);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    modelMat = glm::rotate(modelMat, -0.01f, glm::vec3(0.f, 0.f, 1.f));
    glUniformMatrix4fv(gModelMatLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
    
    glBindVertexArray(gVao);
    
    glDrawArrays(GL_TRIANGLES, 0, teapot.m_Info.vertCount);
}

//OpenGL Shutdown Logic
void glShutdown() {
    glDeleteVertexArrays(1, &gVao);
    
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

