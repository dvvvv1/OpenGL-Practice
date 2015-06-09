//Author: Puzhi Yao
//Date: 18 May 2015
//Program description:
//CG Assignment 3 SKY GLASS KITCHEN

#include "lightSource.h"
#include <iostream>
#include <stdlib.h>
#include <iomanip>
#include <stdio.h>
#include <fstream>
#include <string>
#include <time.h>
#include <vector>
#include "tiny_obj_loader.h"

#include <GL/glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace std;

lightSource::lightSource(int ProgramID, std::vector<float> lightAm, std::vector<float> lightDi, std::vector<float> lightSp, int NUM_LIGHT)
{
    NUM_LIGHT_SOURCES = NUM_LIGHT;
    int lightambientHandle = glGetUniformLocation(ProgramID, "light_ambient");
    int lightdiffuseHandle = glGetUniformLocation(ProgramID, "light_diffuse");
    int lightspecularHandle = glGetUniformLocation(ProgramID, "light_specular");

    if (lightambientHandle == -1 || lightdiffuseHandle == -1 || lightspecularHandle == -1){
        fprintf(stderr, "Error: can't find light uniform variables\n");
        exit(1);
    }
    this->ProgramID = ProgramID;
    lightambient = new float[NUM_LIGHT_SOURCES * 3];
    lightdiffuse = new float[NUM_LIGHT_SOURCES * 3];
    lightspecular = new float[NUM_LIGHT_SOURCES * 3];
    for(int i = 0; i < NUM_LIGHT_SOURCES; i++)
    {
        this->lightambient[i*3] = lightAm[0];
        this->lightambient[i*3+1] = lightAm[1];
        this->lightambient[i*3+1] = lightAm[2];
    }

    for(int i = 0; i < NUM_LIGHT_SOURCES; i++)
    {
        this->lightdiffuse[i*3] = lightDi[0];
        this->lightdiffuse[i*3+1] = lightDi[1];
        this->lightdiffuse[i*3+2] = lightDi[2];
    }

    for(int i = 0; i < NUM_LIGHT_SOURCES; i++)
    {
        this->lightspecular[i*3] = lightSp[0];
        this->lightspecular[i*3+1] = lightSp[1];
        this->lightspecular[i*3+2] = lightSp[2];
    }

    glUniform3fv(lightambientHandle, NUM_LIGHT_SOURCES, lightambient);
    glUniform3fv(lightdiffuseHandle, NUM_LIGHT_SOURCES, lightdiffuse);
    glUniform3fv(lightspecularHandle, NUM_LIGHT_SOURCES, lightspecular);

    this->lightPos = new float[NUM_LIGHT_SOURCES*3];

}

void lightSource::setLightPos(std::vector<glm::vec3> pos)
{
    for(int i = 0; i < pos.size();i++)
    {
        lightPos[i*3] = pos[i][0];
        lightPos[i*3+1] = pos[i][1];
        lightPos[i*3+2] = pos[i][2];

    }
}

void lightSource::lightRender()
{
    int lightposHandle = glGetUniformLocation(this->ProgramID, "LightPosition_worldspace");
    if(lightposHandle == -1 ){
            fprintf(stderr, "Error: can't find light pos matrix uniforms\n");
            exit(1);
        }
    glUniform3fv(lightposHandle, NUM_LIGHT_SOURCES, lightPos);
}

void lightSource::lightRender(std::vector<glm::vec3> pos)
{
    int lightposHandle = glGetUniformLocation(this->ProgramID, "LightPosition_worldspace");
    if(lightposHandle == -1 ){
            fprintf(stderr, "Error: can't find light pos matrix uniforms\n");
            exit(1);
        }
    for(int i = 0; i < pos.size();i++)
    {
        lightPos[i*3] = pos[i][0];
        lightPos[i*3+1] = pos[i][1];
        lightPos[i*3+2] = pos[i][2];

    }
}
