//
//  ModelParser.cpp
//  Template
//
//  Created by Colin Tan on 5/4/15.
//  Copyright (c) 2015 Colin Tan. All rights reserved.
//

#include "Model.h"

#include <assert.h>
#include <fstream>

#include <iostream>

#include "glm/glm.hpp"

enum ModelDataFlag {
    kModelNone,
    kModelVert,
    kModelTex,
    kModelNorm,
    kModelFace
};

Model::~Model() {
    
}

void Model::Transform(glm::mat4 mat) {
    for (auto& e: m_Info.v) {
        e = mat * e;
    }
    
    glm::mat4 normMat = glm::transpose(glm::inverse(mat));
    
    for (auto& e: m_Info.vn) {
        e = glm::vec3(normMat * glm::vec4(e, 1.f));
    }
}

ModelFactory::ModelFactory() {
    
}

ModelFactory::~ModelFactory() {
    
}

Model ModelFactory::BuildCube(float length) {
    Model model {};
    
    float n = length / 2;
    
    /*model.m_Info.v = {
        //Positive yz plane
        {n, n, n, 1.f},
        {n, -n, n, 1.f},
        {n, -n, -n, 1.f},
        {n, n, -n, 1.f},
        
        //Negative yz plane
        {-n, n, -n, 1.f},
        {-n, -n, -n, 1.f},
        {-n, -n, n, 1.f},
        {-n, n, n, 1.f},
        
        //Positive xz plane
        {-n, n, -n, 1.f},
        {-n, n, n, 1.f},
        {n, n, n, 1.f},
        {n, n, -n, 1.f},
        
        //Negative xz plane
        {n, -n, n, 1.f},
        {n, -n, -n, 1.f},
        {-n, -n, -n, 1.f},
        {-n, -n, n, 1.f},
        
        //Positive xy plane
        {-n, n, n, 1.f},
        {-n, -n, n, 1.f},
        {n, -n, n, 1.f},
        {n, n, n, 1.f},
        
        //Negative xy Plane
        {n, n, -n, 1.f},
        {n, -n, -n, 1.f},
        {-n, -n, -n, 1.f},
        {-n, n, -n, 1.f},
    };
    
    model.m_Info.vn = {
        {1.f, 0.f, 0.f},
        {1.f, 0.f, 0.f},
        {1.f, 0.f, 0.f},
        {1.f, 0.f, 0.f},
        
        {-1.f, 0.f, 0.f},
        {-1.f, 0.f, 0.f},
        {-1.f, 0.f, 0.f},
        {-1.f, 0.f, 0.f},
        
        {0.f, 1.f, 0.f},
        {0.f, 1.f, 0.f},
        {0.f, 1.f, 0.f},
        {0.f, 1.f, 0.f},
        
        {0.f, -1.f, 0.f},
        {0.f, -1.f, 0.f},
        {0.f, -1.f, 0.f},
        {0.f, -1.f, 0.f},
        
        {0.f, 0.f, 1.f},
        {0.f, 0.f, 1.f},
        {0.f, 0.f, 1.f},
        {0.f, 0.f, 1.f},
        
        {0.f, 0.f, -1.f},
        {0.f, 0.f, -1.f},
        {0.f, 0.f, -1.f},
        {0.f, 0.f, -1.f}
    };
    
    model.m_Info.f = {
        {0, 1, 3},
        {3, 1, 2},
        {4, 5, 7},
        {7, 5, 6},
        
        {8, 9, 11},
        {11, 9, 10},
        {12, 13, 15},
        {15, 13, 14},
        
        {16, 17, 19},
        {19, 17, 18},
        {20, 21, 23},
        {23, 21, 22}
    };
     
    model.m_Info.faceCount = 12;*/
    
    return model;
}

Model ModelFactory::BuildModel(std::string filename) {
    ModelInfo info;
    ModelDataFlag flag = kModelNone;
    
    std::ifstream in(filename, std::ios::in);
    assert(!in.bad());
    
    std::string str;
    std::vector<double> num;

    glm::vec4 v;
    glm::vec3 vn;
    glm::vec2 vt;
    
    std::vector<glm::vec4> vert;
    std::vector<glm::vec2> tex;
    std::vector<glm::vec3> norm;
    
    vert.push_back(glm::vec4(0.f));
    tex.push_back(glm::vec2(0.f));
    norm.push_back(glm::vec3(0.f));
    
    std::vector<unsigned int> vertIndex;
    std::vector<unsigned int> texIndex;
    std::vector<unsigned int> normIndex;
    
    char faceStr[256];
    unsigned int indices[16];
    
    while(!in.eof()) {
        in >> str;

        if (str == "v") {
            in >> v.x >> v.y >> v.z;
            v.w = 1;
            vert.push_back(v);
        }
        else if (str == "vn") {
            in >> vn.x >> vn.y >> vn.z;
            norm.push_back(vn);
        }
        else if (str == "vt") {
            in >> vt.x >> vt.y;
            tex.push_back(vt);
        }
        else if (str == "f") {
            in.getline(faceStr, 256);
            char* tok = strtok(faceStr, " /\r");
            int count = 0;
            while (tok != NULL) {
                indices[count] = atoi(tok);
                ++count;
                tok = strtok(NULL, " /\r");
            }
            int faces = count / 3;
            assert(faces == 3 || faces == 4);
            if (faces == 3) {
                for (int i = 0; i < 3; i++) {
                    int index = i * 3;
                    vertIndex.push_back(indices[index]);
                    texIndex.push_back(indices[index+1]);
                    normIndex.push_back(indices[index+2]);
                }
            }
            else if (faces == 4) {
                vertIndex.push_back(indices[0]);
                texIndex.push_back(indices[1]);
                normIndex.push_back(indices[2]);
                
                vertIndex.push_back(indices[3]);
                texIndex.push_back(indices[4]);
                normIndex.push_back(indices[5]);
                
                vertIndex.push_back(indices[6]);
                texIndex.push_back(indices[7]);
                normIndex.push_back(indices[8]);
                
                vertIndex.push_back(indices[6]);
                texIndex.push_back(indices[7]);
                normIndex.push_back(indices[8]);
                
                vertIndex.push_back(indices[9]);
                texIndex.push_back(indices[10]);
                normIndex.push_back(indices[11]);
                
                vertIndex.push_back(indices[0]);
                texIndex.push_back(indices[1]);
                normIndex.push_back(indices[2]);
            }
        }
    }
    
    for (int i = 0; i < vertIndex.size(); i++) {
        info.v.push_back(vert[vertIndex[i]]);
        info.vt.push_back(tex[texIndex[i]]);
        info.vn.push_back(norm[normIndex[i]]);
    }
    
    info.vertCount = (unsigned int)vertIndex.size();

    Model model {};
    model.m_Info = info;
    
    in.close();
    
    return model;
}

Model ModelFactory::BuildSheet(float xLength, float zLength, int xDiv, int zDiv) {
    // Start from the corner with positive x and z
    float xStart = xLength / 2.f;
    float zStart = zLength / 2.f;
    
    // Change in positions of x and z
    float dx = xLength / (float)xDiv;
    float dz = zLength / (float)zDiv;
    
    // Change in texture coordinates of s and t
    float ds = 1.f / (float)xDiv;
    float dt = 1.f / (float)zDiv;
    
    Model model {};
    
    for (int i = 0; i < xDiv; i++) {
        for (int j = 0; j < zDiv; j++) {
            /*model.m_Info.v.push_back(glm::vec4(xStart - i * dx, 0.f, zStart - j * dz, 1.f));
            model.m_Info.v.push_back(glm::vec4(xStart - i * dx, 0.f, zStart - (j + 1) * dz, 1.f));
            model.m_Info.v.push_back(glm::vec4(xStart - (i + 1) * dx, 0.f, zStart - j * dz, 1.f));
            model.m_Info.v.push_back(glm::vec4(xStart - (i + 1) * dx, 0.f, zStart - j * dz, 1.f));
            model.m_Info.v.push_back(glm::vec4(xStart - i * dx, 0.f, zStart - (j + 1) * dz, 1.f));
            model.m_Info.v.push_back(glm::vec4(xStart - (i + 1) * dx, 0.f, zStart - (j + 1) * dz, 1.f));*/
            
            model.m_Info.v.push_back(glm::vec4(xStart - i * dx, 0.f, zStart - j * dz, 1.f));
            model.m_Info.v.push_back(glm::vec4(xStart - (i + 1) * dx, 0.f, zStart - j * dz, 1.f));
            model.m_Info.v.push_back(glm::vec4(xStart - i * dx, 0.f, zStart - (j + 1) * dz, 1.f));
            model.m_Info.v.push_back(glm::vec4(xStart - i * dx, 0.f, zStart - (j + 1) * dz, 1.f));
            model.m_Info.v.push_back(glm::vec4(xStart - (i + 1) * dx, 0.f, zStart - j * dz, 1.f));
            model.m_Info.v.push_back(glm::vec4(xStart - (i + 1) * dx, 0.f, zStart - (j + 1) * dz, 1.f));
            
            
            // (0,0) texture coords starts at corner with positive x and z
            model.m_Info.vt.push_back(glm::vec2(i * ds, j * dt));
            model.m_Info.vt.push_back(glm::vec2((i + 1) * ds, j * dt));
            model.m_Info.vt.push_back(glm::vec2(i * ds, (j + 1) * dt));
            model.m_Info.vt.push_back(glm::vec2(i * ds, (j + 1) * dt));
            model.m_Info.vt.push_back(glm::vec2((i + 1) * ds, j * dt));
            model.m_Info.vt.push_back(glm::vec2((i + 1) * ds, (j + 1) * dt));
            
            for (int k = 0; k < 6; k++)
                model.m_Info.vn.push_back(glm::vec3(0.f, 1.f, 0.f));
        }
    }
    
    model.m_Info.vertCount = xDiv * zDiv * 6;
    
    return model;
}