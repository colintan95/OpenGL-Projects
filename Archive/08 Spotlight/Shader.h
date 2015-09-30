//
//  Shader.h
//  Template
//
//  Created by Colin Tan on 10/4/15.
//  Copyright (c) 2015 Colin Tan. All rights reserved.
//

#ifndef __Template__Shader__
#define __Template__Shader__

#include <fstream>
#define GLFW_INCLUDE_GLCOREARB
#include "glfw3.h"

GLuint CreateShader(GLenum type, const char* filename);

void LoadShaderSource(const char* filename, char** source, unsigned long* length);
void FreeShaderSource(char** source);

struct FileInfo {
    char* data;
    size_t size;
};

FileInfo* LoadFile(const char* filename);
void FreeFile(FileInfo* info);

#endif /* defined(__Template__Shader__) */
