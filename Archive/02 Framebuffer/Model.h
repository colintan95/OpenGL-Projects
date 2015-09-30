//
//  ModelParser.h
//  Template
//
//  Created by Colin Tan on 5/4/15.
//  Copyright (c) 2015 Colin Tan. All rights reserved.
//

#ifndef __Template__ModelParser__
#define __Template__ModelParser__

#include <stdio.h>

#include <string>

#include <vector>
#include "glm/glm.hpp"

class ModelFactory;

struct ModelInfo {
    std::vector<glm::vec4> v;
    std::vector<glm::vec3> vn;
    std::vector<glm::vec2> vt;
    
    std::vector<glm::uvec3> fv;
};

class Model {
    friend class ModelFactory;
    
public:
    Model() {}
    ~Model();
    
    void Transform(glm::mat4 mat);
    
protected:
    
public:
    ModelInfo m_Info;
    int m_NumFace = 0;
    int m_NumVert = 0;
};


class ModelFactory {
    
public:
    ModelFactory();
    ~ModelFactory();
    
    Model BuildCube(float length);
    Model BuildModel(std::string filename);
    
private:
    
};

#endif /* defined(__Template__ModelParser__) */
