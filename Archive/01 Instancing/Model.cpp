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
    
    model.m_Info.v = {
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
    
    model.m_Info.fv = {
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
    
    model.m_NumFace = 12;
    model.m_NumVert = 24;

    return model;
}

Model ModelFactory::BuildModel(std::string filename) {
    ModelInfo info;
    
    std::ifstream in(filename, std::ios::in);
    assert(!in.bad());
    
    char str[256];
    std::vector<double> num;
    
    while(!in.eof()) {
        in.getline(str, 256);

        char* tok;
        
        tok = strtok(str, " /");
        
        while (tok != NULL) {
            if (strcmp(tok, "v") == 0) {
                
            }
            
            tok = strtok(NULL, " /");
        }
    }
    
    Model model {};
    model.m_Info = info;
    
    in.close();
    
    return model;
}