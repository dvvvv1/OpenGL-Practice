//Author: pz.yao
//Date: 18 May 2015
//Program description:
//CG Assignment 3 SKY GLASS KITCHEN

/**
 This class will auto create light source
*/
#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

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

class lightSource
{
public: 
        int ProgramID;
        int NUM_LIGHT_SOURCES;
        float* lightambient;
        float* lightdiffuse;
        float* lightspecular;
        float* lightPos;
   

    lightSource(int ProgramID, std::vector<float> lightAm, std::vector<float> lightDi, std::vector<float> lightSp, int NUM_LIGHT);
    void setLightPos(std::vector<glm::vec3> pos);
    void lightRender();
    void lightRender(std::vector<glm::vec3> pos);
  
};
#endif
