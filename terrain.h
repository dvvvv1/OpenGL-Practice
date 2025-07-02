//Author: pz.yao
//Date: 18 May 2015
//Program description:
//CG Assignment 3 SKY GLASS KITCHEN

/**
 This class will auto create VAO by model data
*/
#ifndef TERRAIN_H
#define TERRAIN_H

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


class Terrain
{
private: 
    int mapSize;
    int totNumPoints;
    int indexNum;
    int totNumOfIndices;
    int sideNodes;
    float width;
    float* nodes;
    int *indices;
    float *texCoord;
    float *normals;
    unsigned int terrainVaoHandle;
    int Vcount;
    int Ccount;
    int Tcount;
    int Ncount;
    int Icount;
    float lightambient[3];
    float lightdiffuse[3];
    float lightspecular[3];
    float* lightPos;
   
public:
    Terrain();
    int  createTerrianVAO(unsigned int ProgramID, bool checkM);
    void render(unsigned int ProgramID, float fs, bool checkM, int mode, std::vector<float> scaleVec, glm::vec3 position, float rotAngle, glm::vec3 rotAxis, float shin);
    std::vector<float> autoCalNormals(std::vector<float> v1, std::vector<float> v2, std::vector<float> v3);
    std::vector<int> findRelativeVertex(int index);
    unsigned int getTerrainVAOHandle();
    int getTotNumOfIndices();
  
};
#endif
