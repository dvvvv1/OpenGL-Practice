//Modified by: Puzhi Yao
//Date: 18 May 2015
//Program description:
//CG Assignment 3 SKY GLASS KITCHEN

/*********************************************************
 Version 1.1
 
 Code provided by Michael Hemsley, Anthony Dick and Gustavo Carneiro
 for the course 
 COMP SCI 3014/7090 Computer Graphics
 School of Computer Science
 University of Adelaide
 
 Permission is granted for anyone to copy, use, modify, or distribute this
 program and accompanying programs and documents for any purpose, provided
 this copyright notice is retained and prominently displayed, along with
 a note saying that the original programs are available from the aforementioned 
 course web page. 
 
 The programs and documents are distributed without any warranty, express or
 implied.  As the programs were written for research purposes only, they have
 not been tested to the degree that would be advisable in any important
 application.  All use of these programs is entirely at the user's own risk.
 *********************************************************/

/*
 * sphere.cpp
 *
 * Using method described:
 * http://stackoverflow.com/questions/5988686/how-do-i-create-a-3d-sphere-in-opengl-using-visual-c
 * and various other places.
 */

#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <iomanip>
#include <stdio.h>
#include <fstream>
#include <string>
#include <time.h>
#include <vector>
#include "tiny_obj_loader.h"

#include "Sphere.hpp"

using namespace std;

Sphere::Sphere(float radius, int vertDiv, int horzDiv)
{
	// Force minimum values
	if (vertDiv < 1) vertDiv = 1;
	if (horzDiv < 4) horzDiv = 4;

	vertDiv = vertDiv+2;	// Add 2 for top and bottom-cap 'divisions'
	float V = 1.0/(float)(vertDiv-1);
	float H = 1.0/(float)(horzDiv-1);
	int v, h;

	vertices = new float[vertDiv*horzDiv*3];
	normals = new float[vertDiv*horzDiv*3];
    texCoords = new float[vertDiv*horzDiv*2];
	indices = new unsigned int[vertDiv*horzDiv*6];
    colours = new float[vertDiv*horzDiv*4];


	vertCount = 0;
	normCount = 0;
    texCount = 0;
    indCount = 0;
    colorCount = 0;

	// Construct vertex, normal and tex-coords
	for (v=0; v<vertDiv; v++) {
        for (h=0; h<horzDiv; h++) {
            float x = cos(2*M_PI * h * H) * sin(M_PI * v * V);
            float y = sin(-M_PI_2 + M_PI * v * V);
            float z = sin(2*M_PI * h * H) * sin(M_PI * v * V);

            vertices[vertCount++] = x*radius;
            vertices[vertCount++] = y*radius;
            vertices[vertCount++] = z*radius;

            normCount++;
            normCount++;
            normCount++;

            texCoords[texCount++] = h*H;
            texCoords[texCount++] = v*V;

            colours[colorCount++] = 1.0f;
            colours[colorCount++] = 1.0f;
            colours[colorCount++] = 1.0f;
            colours[colorCount++] = 1.0f;
        }
    }
    
	// Construct indices
	// Start with special case of bottom sub-division where all vertices meet
	v = 0;
	for (h=0; h<horzDiv-1; h++) {
		indices[indCount++] = v * horzDiv + h;
		indices[indCount++] = (v+1) * horzDiv + h;
		indices[indCount++] = (v+1) * horzDiv + (h+1);
	}

	// Middle divisions
	for (v=1; v<vertDiv-2; v++) {
        for (h=0; h<horzDiv-1; h++) {

            indices[indCount++] = v * horzDiv + h;
            indices[indCount++] = (v+1) * horzDiv + (h+1);
            indices[indCount++] = v * horzDiv + (h+1);

            indices[indCount++] = v * horzDiv + h;
            indices[indCount++] = (v+1) * horzDiv + h;
            indices[indCount++] = (v+1) * horzDiv + (h+1);
        }
	}

	// Cap off the top of the sphere
	v = vertDiv-2;
	for (h=0; h<horzDiv-1; h++) {
		indices[indCount++] = v * horzDiv + h;
		indices[indCount++] = (v+1) * horzDiv + (h+1);
		indices[indCount++] = v * horzDiv + (h+1);
	}

    // auto generate normals
    for(int j = 0; j < normCount/3; j++)
    {
        vector<int> relativeV = findRelativeVertex(j);
        vector<float> sumNormal;
        float v1_sum = 0;
        float v2_sum = 0;
        float v3_sum = 0;
        sumNormal.push_back(v1_sum);
        sumNormal.push_back(v2_sum);
        sumNormal.push_back(v3_sum);
        vector<float> NormalBackup;
        float B1 = 0.0f;
        float B2 = 0.0f;
        float B3 = 0.0f;
        NormalBackup.push_back(B1);
        NormalBackup.push_back(B2);
        NormalBackup.push_back(B3);
        // use relative vertices to calculate normal for face
        for(int k = 0; k < relativeV.size()/3; k++)
        {
            float v1_position1 = this->vertices[(relativeV[k*3])*3];
            float v1_position2 = this->vertices[(relativeV[k*3])*3+1];
            float v1_position3 = this->vertices[(relativeV[k*3])*3+2];

            float v2_position1 = this->vertices[(relativeV[k*3+1])*3];
            float v2_position2 = this->vertices[(relativeV[k*3+1])*3+1];
            float v2_position3 = this->vertices[(relativeV[k*3+1])*3+2];

            float v3_position1 = this->vertices[(relativeV[k*3+2])*3];
            float v3_position2 = this->vertices[(relativeV[k*3+2])*3+1];
            float v3_position3 = this->vertices[(relativeV[k*3+2])*3+2];

            vector<float> v1;
            vector<float> v2;
            vector<float> v3;

            v1.push_back(v1_position1);
            v1.push_back(v1_position2);
            v1.push_back(v1_position3);
            v2.push_back(v2_position1);
            v2.push_back(v2_position2);
            v2.push_back(v2_position3);
            v3.push_back(v3_position1);
            v3.push_back(v3_position2);
            v3.push_back(v3_position3);

            vector<float> oneNormal = autoCalNormals(v1,v2,v3);
            // sum all relative normal shared by one vetex
            sumNormal[0] = sumNormal[0] + oneNormal[0];
            sumNormal[1] = sumNormal[1] + oneNormal[1];
            sumNormal[2] = sumNormal[2] + oneNormal[2];
            if(sumNormal[0] != 0)
            {
                NormalBackup[0] = sumNormal[0];
            }
            if(sumNormal[1] != 0)
            {
                NormalBackup[1] = sumNormal[1];
            }
            if(sumNormal[2] != 0)
            {
                NormalBackup[2] = sumNormal[2];
            }
        }

        // normalize sum of normal and calculate ave normal
        float sumNorm = sqrt((sumNormal[0]*sumNormal[0])+(sumNormal[1]*sumNormal[1])+(sumNormal[2]*sumNormal[2]));
        float backNorm = sqrt((NormalBackup[0]*NormalBackup[0])+(NormalBackup[1]*NormalBackup[1])+(NormalBackup[2]*NormalBackup[2]));
        if(sumNorm == 0)
        {
            sumNormal[0] = NormalBackup[0]/backNorm;
            sumNormal[1] = NormalBackup[1]/backNorm;
            sumNormal[2] = NormalBackup[2]/backNorm;
            if(backNorm == 0)
            {
                sumNormal[0] = 1.0f;
                sumNormal[1] = 1.0f;
                sumNormal[2] = 1.0f;
            }
        }
        else{
            sumNormal[0] = sumNormal[0]/sumNorm;
            sumNormal[1] = sumNormal[1]/sumNorm;
            sumNormal[2] = sumNormal[2]/sumNorm;
        }
        normals[j*3] = sumNormal[0];
        normals[j*3+1] = sumNormal[1];
        normals[j*3+2] = sumNormal[2];
    }
}

std::vector<float> Sphere::autoCalNormals(std::vector<float> v1, std::vector<float> v2, std::vector<float> v3)
{
    vector<float> resultNormal;
    float normal_x = 0;
    float normal_y = 0;
    float normal_z = 0;
    resultNormal.push_back(normal_x);
    resultNormal.push_back(normal_y);
    resultNormal.push_back(normal_z);

    vector<float> U;
    vector<float> V;
    for(int i = 0; i < 3; i++)
    {
        float tempU = 0;
        float tempV = 0;
        U.push_back(tempU);
        V.push_back(tempV);
    }

    // Set vector U to p2 - p1
    U[0] = v2[0] - v1[0];
    U[1] = v2[1] - v1[1];
    U[2] = v2[2] - v1[2];
    // Set vector V to p3 - p1
    V[0] = v3[0] - v1[0];
    V[1] = v3[1] - v1[1];
    V[2] = v3[2] - v1[2];

    // Set Normal.x to (multiply U.y by V.z) minus (multiply U.z by V.y)
    resultNormal[0] = U[1] * V[2] - U[2] * V[1];
    // Set Normal.y to (multiply U.z by V.x) minus (multiply U.x by V.z)
    resultNormal[1] = U[2] * V[0] - U[0] * V[2];
    // Set Normal.z to (multiply U.x by V.y) minus (multiply U.y by V.x)
    resultNormal[2] = U[0] * V[1] - U[1] * V[0];

    // Normalizing
    float norm = sqrt((resultNormal[0]*resultNormal[0]) + (resultNormal[1]*resultNormal[1]) + (resultNormal[2]*resultNormal[2]));
    if(norm == 0)
    {
        resultNormal[0] = 0.0f;
        resultNormal[0] = 0.0f;
        resultNormal[0] = 0.0f;
    }
    else{
        resultNormal[0] = resultNormal[0]/norm;
        resultNormal[1] = resultNormal[1]/norm;
        resultNormal[2] = resultNormal[2]/norm;
    }
    return resultNormal;
}

/**
 * @brief modelDataVAO::findRelativeVertex
 * @param index
 * @return vector<float>
 * This function is used to find surface of shared vertex.
 */
std::vector<int> Sphere::findRelativeVertex(int index)
{
    vector<int> relativeIndices;
    for(int i = 0; i < this->indCount/3; i++)
    {
        if(this->indices[i*3] == index
                || this->indices[i*3+1] == index
                || this->indices[i*3+2] == index)
        {
            int v1 = this->indices[i*3];
            int v2 = this->indices[i*3+1];
            int v3 = this->indices[i*3+2];
            relativeIndices.push_back(v1);
            relativeIndices.push_back(v2);
            relativeIndices.push_back(v3);
        }
    }
    return relativeIndices;
}

