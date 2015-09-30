//
//  Shader.cpp
//  Template
//
//  Created by Colin Tan on 10/4/15.
//  Copyright (c) 2015 Colin Tan. All rights reserved.
//

#include "Shader.h"
#include <iostream>
#include <assert.h>

GLuint CreateShader(GLenum type, const char* filename) {
    GLuint shader = glCreateShader(type);
    
    char* src;
    unsigned long srcLen;
    
    LoadShaderSource(filename, &src, &srcLen);
    
    GLint logLength;
    GLint compileStatus;
    
    glShaderSource(shader, 1, &src, NULL);
    
    glCompileShader(shader);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar* log = (GLchar*)malloc(sizeof(GLchar) * logLength);
        glGetShaderInfoLog(shader, logLength, &logLength, log);
        std::cout << "Shader Info Log from " << filename << ": "<< std::endl;
        std::cout << log << std::endl;
        free(log);
    }
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    assert(compileStatus);
    
    FreeShaderSource(&src);
    
    return shader;
}

void LoadShaderSource(const char* filename, char** source, unsigned long* length) {
    std::ifstream in;
    in.open(filename, std::ios::in);
    
    assert(in.good());
    
    unsigned long len;
    in.seekg(0, std::ios::end);
    len = in.tellg();
    in.seekg(0, std::ios::beg);
    
    (*source) = new char[len+1];
    int i = 0;
    while (!in.eof()) {
        (*source)[i] = in.get();
        i++;
    }
    
    (*source)[i-1] = 0;
    *length = len;
    
    in.close();
}

void FreeShaderSource(char** source) {
    delete[] (*source);
}

FileInfo* LoadFile(const char* filename) {
    FileInfo* info = (FileInfo*)malloc(sizeof(FileInfo));
    
    FILE* file = fopen(filename, "rb");
    
    size_t size;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    assert(size != 0);
    
    info->data = (char*)malloc(sizeof(char) * (size + 1));
    
    fread(info->data, 1, size, file);
    info->data[size] = 0;
    
    fclose(file);
    
    info->size = size;
    
    return info;
}

void FreeFile(FileInfo* info) {
    free(info->data);
    free(info);
}