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

GLuint gMipmapProg;
GLuint gMipmapVS;
GLuint gMipmapFS;

GLuint gMipmapVao;

GLuint gMipmapPosBuf;
GLuint gMipmapTexCoordBuf;

GLuint gMipmapTexture;

GLint gMipmapPosLoc;
GLint gMipmapTexCoordLoc;

GLint gMipmapModelMatLoc;
GLint gMipmapViewMatLoc;
GLint gMipmapProjMatLoc;
GLint gMipmapTextureLoc;

//OpenGL Constant Variables

const GLfloat PROJ_NEAR = 0.1f;
const GLfloat PROJ_FAR = 1000.f;

//Other User-defined Global Variables

glm::mat4 modelMat(1.f);

glm::mat4 gViewMat(1.f);
glm::mat4 gProjMat(1.f);

unsigned char* gMipmapTexImage;


//OpenGL Initialization Logic
void glInit() {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    gViewMat = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -10.f, 0.f));
    gProjMat = glm::perspective(45.f, (float)gScreenWidth / (float)gScreenHeight, PROJ_NEAR, PROJ_FAR);
    
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    gMipmapProg = glCreateProgram();
    gMipmapVS = CreateShader(GL_VERTEX_SHADER, "mipmap.vert");
    gMipmapFS = CreateShader(GL_FRAGMENT_SHADER, "mipmap.frag");
    glAttachShader(gMipmapProg, gMipmapVS);
    glAttachShader(gMipmapProg, gMipmapFS);
    glLinkProgram(gMipmapProg);
    glUseProgram(gMipmapProg);
    
    int imgWidth, imgHeight, imgComp;
    gMipmapTexImage = stbi_load("mipmap.bmp", &imgWidth, &imgHeight, &imgComp, STBI_rgb);
    glGenTextures(1, &gMipmapTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gMipmapTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); //Linear filtering for intra-mipmap, nearest filtering for inter-mipmap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 1); //Sets minimum mipmap level to 1
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4); //Sets maximium mipmap level to 4
    
    glPixelStorei(GL_UNPACK_ROW_LENGTH, imgWidth); //Required for subimaging as OpenGL can obtain the location of the next row in the image
    
    //Sets textures at each mipmap level
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, gMipmapTexImage);
    
    unsigned char* mipImg1 = gMipmapTexImage + (0 * imgWidth + 128) * 3; //Location of subimage
    glTexImage2D(GL_TEXTURE_2D, 1, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, mipImg1);
    
    unsigned char* mipImg2 = gMipmapTexImage + (64 * imgWidth + 128) * 3;
    glTexImage2D(GL_TEXTURE_2D, 2, GL_RGB, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, mipImg2);
    
    unsigned char* mipImg3 = gMipmapTexImage + (96 * imgWidth + 128) * 3;
    glTexImage2D(GL_TEXTURE_2D, 3, GL_RGB, 16, 16, 0, GL_RGB, GL_UNSIGNED_BYTE, mipImg3);
    
    unsigned char* mipImg4 = gMipmapTexImage + (112 * imgWidth + 128) * 3;
    glTexImage2D(GL_TEXTURE_2D, 4, GL_RGB, 8, 8, 0, GL_RGB, GL_UNSIGNED_BYTE, mipImg4);
    
    unsigned char* mipImg5 = gMipmapTexImage + (120 * imgWidth + 128) * 3;
    glTexImage2D(GL_TEXTURE_2D, 5, GL_RGB, 4, 4, 0, GL_RGB, GL_UNSIGNED_BYTE, mipImg5);
    
    unsigned char* mipImg6 = gMipmapTexImage + (124 * imgWidth + 128) * 3;
    glTexImage2D(GL_TEXTURE_2D, 6, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, mipImg6);
    
    unsigned char* mipImg7 = gMipmapTexImage + (126 * imgWidth + 128) * 3;
    glTexImage2D(GL_TEXTURE_2D, 7, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, mipImg7);
    
    gMipmapModelMatLoc = glGetUniformLocation(gMipmapProg, "uModelMat");
    glm::mat4 modelMat = glm::mat4(1.f);
    glUniformMatrix4fv(gMipmapModelMatLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
    
    gMipmapViewMatLoc = glGetUniformLocation(gMipmapProg, "uViewMat");
    glUniformMatrix4fv(gMipmapViewMatLoc, 1, GL_FALSE, glm::value_ptr(gViewMat));
    
    gMipmapProjMatLoc = glGetUniformLocation(gMipmapProg, "uProjMat");
    glUniformMatrix4fv(gMipmapProjMatLoc, 1, GL_FALSE, glm::value_ptr(gProjMat));
    
    gMipmapTextureLoc = glGetUniformLocation(gMipmapProg, "uTexture");
    glUniform1i(gMipmapTextureLoc, 0);
    
    glGenBuffers(1, &gMipmapPosBuf);
    glBindBuffer(GL_ARRAY_BUFFER, gMipmapPosBuf);
    glm::vec4 mipmapPosData[] = {
        {-1000.f, 0.f, -1000.f, 1.f}, {-1000.f, 0.f, 1000.f, 1.f}, {1000.f, 0.f, -1000.f, 1.f},
        {1000.f, 0.f, -1000.f, 1.f}, {-1000.f, 0.f, 100.f, 1.f}, {1000.f, 0.f, 1000.f, 1.f}
    };
    glBufferData(GL_ARRAY_BUFFER, 6 * 4 * sizeof(float), &mipmapPosData[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &gMipmapTexCoordBuf);
    glBindBuffer(GL_ARRAY_BUFFER, gMipmapTexCoordBuf);
    glm::vec2 mipmapTexCoordData[] = {
        {-10.f, -10.f}, {-10.f, 10.f}, {10.f, -10.f},
        {10.f, -10.f}, {-10.f, 10.f}, {10.f, 10.f}
    };
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), mipmapTexCoordData, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &gMipmapVao);
    glBindVertexArray(gMipmapVao);
    
    gMipmapPosLoc = glGetAttribLocation(gMipmapProg, "vPosition");
    glEnableVertexAttribArray(gMipmapPosLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gMipmapPosBuf);
    glVertexAttribPointer(gMipmapPosLoc, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    
    gMipmapTexCoordLoc = glGetAttribLocation(gMipmapProg, "vTexCoord");
    glEnableVertexAttribArray(gMipmapTexCoordLoc);
    glBindBuffer(GL_ARRAY_BUFFER, gMipmapTexCoordBuf);
    glVertexAttribPointer(gMipmapTexCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    
    //Miscellaneous
}

//OpenGL Loop Logic

void glLoop() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(gMipmapProg);
    glBindVertexArray(gMipmapVao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
}

//OpenGL Shutdown Logic
void glShutdown() {
    
    glDeleteVertexArrays(1, &gMipmapVao);
    glDeleteBuffers(1, &gMipmapTexCoordBuf);
    glDeleteBuffers(1, &gMipmapPosBuf);
    glDeleteTextures(1, &gMipmapTexture);
    glDeleteShader(gMipmapFS);
    glDeleteShader(gMipmapVS);
    glDeleteProgram(gMipmapProg);
    
    stbi_image_free(gMipmapTexImage);
}

static void ResizeCallback(GLFWwindow* window, int width, int height) {
    gScreenWidth = width;
    gScreenHeight = height;
    
    gProjMat = glm::perspective(45.f, (float)width / (float)height, PROJ_NEAR, PROJ_FAR);
    glUniformMatrix4fv(gMipmapProjMatLoc, 1, GL_FALSE, glm::value_ptr(gProjMat));
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

