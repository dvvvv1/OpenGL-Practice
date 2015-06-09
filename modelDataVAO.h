//Author: Puzhi Yao
//Date: 18 May 2015
//Program description:
//CG Assignment 3 SKY GLASS KITCHEN
/**
 This class will auto create VAO by model data
*/
#ifndef MODELDATAVAO_H
#define MODELDATAVAO_H

#include <iostream>
#include <stdlib.h>
#include <iomanip>
#include <stdio.h>
#include <fstream>
#include <string>
#include <time.h>
#include <vector>
#include "tiny_obj_loader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


class modelDataVAO
{
private: 
    int indiceCount;
    int verticeCount;
    int texCoordCount;
    int normalCount;
    int NumOfTex;
    unsigned int squareVaoHandle;
    int mat_ID;
   
public:
    tinyobj::shape_t shape;
    tinyobj::material_t material;
    modelDataVAO(tinyobj::shape_t shapes, tinyobj::material_t materials,int ProgramID);
    int createVAO(int ProgramID, bool checkM);
    int createVAO(int ProgramID, bool checkM, float fs);
    void render(int ProgramID, glm::mat4 mvMatrix, bool checkM, int mode, glm::vec3 position, float rotAngle, glm::vec3 rotAxis);
    glm::mat4 render(int ProgramID,float fs, glm::mat4 mvMatrix, bool checkM, int mode, std::vector<float> scaleVec, glm::vec3 position, std::vector<float> rotAngle, std::vector<glm::vec3> rotAxis);
    std::vector<float> modelSize();
    int getIndiceCount();
    int getVerticeCount();
    int getNormalCount();
    int getNumOfTex();
    unsigned int getVaoHandle();
    int getMatID();
    std::vector<float> autoCalNormals(std::vector<float> v1, std::vector<float> v2, std::vector<float> v3);
    std::vector<float> findRelativeVertex(int index);
};
#endif
