//Author: Puzhi Yao
//Date: 18 May 2015
//Program description:
//CG Assignment 3 SKY GLASS KITCHEN

#include "modelDataVAO.h"
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

#include "Viewer.h"

#define VALS_PER_VERT 3
#define VALS_PER_COLOUR 4
#define VALS_PER_TEX 2          // number of dimensions of the texture data (if image, set it to 2)
#define POLYGON_NUM_TRIS 2      // number of squares in a cube
#define POLYGON_NUM_VERTICES 4     // number of vertices in a cube`
#define VALS_PER_NORMAL 3 // number of normal per vertices

using namespace std;



/**
 * @brief modelDataVAO::createVAO
 * @param ProgramID
 * @param checkM
 * @return
 * This function is used to create VAO per shape, and containing the
 * auto-generate normal function.
 */
int modelDataVAO::createVAO(int ProgramID, bool checkM)
{
    //record # of positions of all shapes
    //record # of indices of all shapes
    this->verticeCount = this->shape.mesh.positions.size();
    this->indiceCount = this->shape.mesh.indices.size();
    this->texCoordCount = this->shape.mesh.texcoords.size();
    this->normalCount = this->shape.mesh.normals.size();

    // Generating object
    // vertices define
    float *vertices = new float[this->verticeCount];
    int tempVCount = 0;

    // Indices defining the way to group vertices into triangles
    unsigned int *indices = new unsigned int[this->indiceCount];
    int tempICount = 0;

    // Normals
    float *normals;
    // check to see if there exists normals
    // if not, auto-generate normals per vertex
    if(this->normalCount == 0)
    {
        normals = new float[this->verticeCount];
        for(int j = 0; j < this->shape.mesh.normals.size()/3; j++)
        {
            vector<float> relativeV = findRelativeVertex(j);
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
                float v1_position1 = this->shape.mesh.positions[(relativeV[k*3])*3];
                float v1_position2 = this->shape.mesh.positions[(relativeV[k*3])*3+1];
                float v1_position3 = this->shape.mesh.positions[(relativeV[k*3])*3+2];

                float v2_position1 = this->shape.mesh.positions[(relativeV[k*3+1])*3];
                float v2_position2 = this->shape.mesh.positions[(relativeV[k*3+1])*3+1];
                float v2_position3 = this->shape.mesh.positions[(relativeV[k*3+1])*3+2];

                float v3_position1 = this->shape.mesh.positions[(relativeV[k*3+2])*3];
                float v3_position2 = this->shape.mesh.positions[(relativeV[k*3+2])*3+1];
                float v3_position3 = this->shape.mesh.positions[(relativeV[k*3+2])*3+2];

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
    else{
        normals= new float[this->normalCount];
    }

    // Tex-coordinate
    float *tex_coord;
    int defTex = 0;
    if(this->texCoordCount == 0)
    {
        defTex = 2*((this->verticeCount)/3);
        tex_coord = new float[defTex];
        for(int i = 0; i < defTex; i++)
        {
            tex_coord[i] = 1.0f;
        }
    }
    else{
        tex_coord = new float[this->texCoordCount];
    }

    int tempTCount = 0;

    for(int j = 0; j < this->shape.mesh.positions.size(); j++){
        vertices[tempVCount] = this->shape.mesh.positions[j];
        tempVCount++;
    }

    for(int j = 0; j < this->shape.mesh.indices.size(); j++){
        indices[tempICount] = this->shape.mesh.indices[j];
        tempICount++;
    }

    // check to see if there exists normals
    // if yes, read exist normals
    if(this->shape.mesh.normals.size() != 0)
    {
        for(int j = 0; j < this->shape.mesh.normals.size()/3; j++)
        {
            // Detect the absence of normals and generate them automatically
            float temp1 = this->shape.mesh.normals[j*3];
            float temp2 = this->shape.mesh.normals[j*3+1];
            float temp3 = this->shape.mesh.normals[j*3+2];
            if(temp1 == 0 && temp2 == 2 && temp3)
            {
                vector<float> relativeV = findRelativeVertex(j);
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
                    float v1_position1 = this->shape.mesh.positions[(relativeV[k*3])*3];
                    float v1_position2 = this->shape.mesh.positions[(relativeV[k*3])*3+1];
                    float v1_position3 = this->shape.mesh.positions[(relativeV[k*3])*3+2];

                    float v2_position1 = this->shape.mesh.positions[(relativeV[k*3+1])*3];
                    float v2_position2 = this->shape.mesh.positions[(relativeV[k*3+1])*3+1];
                    float v2_position3 = this->shape.mesh.positions[(relativeV[k*3+1])*3+2];

                    float v3_position1 = this->shape.mesh.positions[(relativeV[k*3+2])*3];
                    float v3_position2 = this->shape.mesh.positions[(relativeV[k*3+2])*3+1];
                    float v3_position3 = this->shape.mesh.positions[(relativeV[k*3+2])*3+2];

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
            else{
                normals[j*3] = this->shape.mesh.normals[j*3];
                normals[j*3+1] = this->shape.mesh.normals[j*3+1];
                normals[j*3+2] = this->shape.mesh.normals[j*3+2];
            }
        }
    }

    if(this->texCoordCount != 0)
    {
        for(int j = 0; j < this->shape.mesh.texcoords.size(); j++){
            tex_coord[tempTCount] = this->shape.mesh.texcoords[j];
            tempTCount++;
        }
    }

    // set up colours
    float *Colours = new float[this->verticeCount*4];
    for(int i = 0; i < this->verticeCount; i++)
    {
        Colours[i*4] = 1.0f;
        Colours[i*4+1] = 1.0f;
        Colours[i*4+2] = 1.0f;
        Colours[i*4+3] = 1.0f;
    }

    //cout <<"In ModelDataVAO:"<<endl;
    //cout <<"tot vertices: "<<this->verticeCount<<endl;
    //cout <<"tot indices: "<<this->indiceCount<<endl;
    //cout <<"tot tex: "<<this->texCoordCount<<" "<<defTex<<endl;

    // The usual code to send data to GPU and link it to shader variables.
    glUseProgram(ProgramID);
    glGenVertexArrays(1, &squareVaoHandle);
    glBindVertexArray(squareVaoHandle);

    int vertLoc = glGetAttribLocation(ProgramID, "a_vertex");
    int colourLoc = glGetAttribLocation(ProgramID, "a_colour");

    //only works in shader 1
    if(checkM == true)
    {
        int texLoc = glGetAttribLocation(ProgramID, "a_tex_coord");
        int normLoc = glGetAttribLocation(ProgramID, "a_normal");

        // buffers
        unsigned int buffer[5];
        glGenBuffers(5, buffer);

        // Set vertex attribute
        glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*this->verticeCount, vertices, GL_STATIC_DRAW);
        // Interpret
        glEnableVertexAttribArray(vertLoc);
        glVertexAttribPointer(vertLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

        // Set colour attr
        glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
        glBufferData(GL_ARRAY_BUFFER,sizeof(float)*4*this->verticeCount, Colours, GL_STATIC_DRAW);
        glEnableVertexAttribArray(colourLoc);
        glVertexAttribPointer(colourLoc, VALS_PER_COLOUR, GL_FLOAT, GL_FALSE, 0, 0);

        // Texture attributes
        if(this->texCoordCount == 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*defTex, tex_coord, GL_STATIC_DRAW);
            glEnableVertexAttribArray(texLoc);
            glVertexAttribPointer(texLoc, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

        }
        else{
            glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*this->texCoordCount, tex_coord, GL_STATIC_DRAW);
            glEnableVertexAttribArray(texLoc);
            glVertexAttribPointer(texLoc, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

        }

        // Normals
        if(this->normalCount == 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*this->verticeCount, normals, GL_STATIC_DRAW);
            glEnableVertexAttribArray(normLoc);
            glVertexAttribPointer(normLoc, VALS_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);
        }
        else{
            glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*this->normalCount, normals, GL_STATIC_DRAW);
            glEnableVertexAttribArray(normLoc);
            glVertexAttribPointer(normLoc, VALS_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);
        }

        // Array of vertex indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[4]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*this->indiceCount, indices, GL_STATIC_DRAW);

        // Un-bind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    else{
        // buffers
        unsigned int buffer[3];
        glGenBuffers(3, buffer);

        // Set vertex attribute
        glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*this->verticeCount, vertices, GL_STATIC_DRAW);
        // Interpret
        glEnableVertexAttribArray(vertLoc);
        glVertexAttribPointer(vertLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

        // Set colour attr
        glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
        glBufferData(GL_ARRAY_BUFFER,sizeof(float)*4*this->verticeCount, Colours, GL_STATIC_DRAW);
        glEnableVertexAttribArray(colourLoc);
        glVertexAttribPointer(colourLoc, VALS_PER_COLOUR, GL_FLOAT, GL_FALSE, 0, 0);

        // Array of vertex indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[4]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*this->indiceCount, indices, GL_STATIC_DRAW);

        // Un-bind
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);      
    }
}

/**
 * @brief modelDataVAO::createVAO
 * @param ProgramID
 * @param checkM
 * @return
 * This function is used to create VAO per shape, and containing the
 * auto-generate normal function.
 */
int modelDataVAO::createVAO(int ProgramID, bool checkM, float fs)
{
    //record # of positions of all shapes
    //record # of indices of all shapes
    this->verticeCount = this->shape.mesh.positions.size();
    this->indiceCount = this->shape.mesh.indices.size();
    this->texCoordCount = this->shape.mesh.texcoords.size();
    this->normalCount = this->shape.mesh.normals.size();

    // Generating object
    // vertices define
    float *vertices = new float[this->verticeCount];
    int tempVCount = 0;

    // Indices defining the way to group vertices into triangles
    unsigned int *indices = new unsigned int[this->indiceCount];
    int tempICount = 0;

    // Normals
    float *normals;
    // check to see if there exists normals
    // if not, auto-generate normals per vertex
    if(this->normalCount == 0)
    {
        normals = new float[this->verticeCount];
        for(int j = 0; j < this->shape.mesh.normals.size()/3; j++)
        {
            vector<float> relativeV = findRelativeVertex(j);
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
                float v1_position1 = this->shape.mesh.positions[(relativeV[k*3])*3];
                float v1_position2 = this->shape.mesh.positions[(relativeV[k*3])*3+1];
                float v1_position3 = this->shape.mesh.positions[(relativeV[k*3])*3+2];

                float v2_position1 = this->shape.mesh.positions[(relativeV[k*3+1])*3];
                float v2_position2 = this->shape.mesh.positions[(relativeV[k*3+1])*3+1];
                float v2_position3 = this->shape.mesh.positions[(relativeV[k*3+1])*3+2];

                float v3_position1 = this->shape.mesh.positions[(relativeV[k*3+2])*3];
                float v3_position2 = this->shape.mesh.positions[(relativeV[k*3+2])*3+1];
                float v3_position3 = this->shape.mesh.positions[(relativeV[k*3+2])*3+2];

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
    else{
        normals= new float[this->normalCount];
    }

    // Tex-coordinate
    float *tex_coord;
    int defTex = 0;
    if(this->texCoordCount == 0)
    {
        defTex = 2*((this->verticeCount)/3);
        tex_coord = new float[defTex];
        for(int i = 0; i < defTex; i++)
        {
            tex_coord[i] = 1.0f;
        }
    }
    else{
        tex_coord = new float[this->texCoordCount];
    }

    int tempTCount = 0;

    for(int j = 0; j < this->shape.mesh.positions.size(); j++){
        vertices[tempVCount] = this->shape.mesh.positions[j]/fs;
        tempVCount++;
    }

    for(int j = 0; j < this->shape.mesh.indices.size(); j++){
        indices[tempICount] = this->shape.mesh.indices[j];
        tempICount++;
    }

    // check to see if there exists normals
    // if yes, read exist normals
    if(this->shape.mesh.normals.size() != 0)
    {
        for(int j = 0; j < this->shape.mesh.normals.size()/3; j++)
        {
            // Detect the absence of normals and generate them automatically
            float temp1 = this->shape.mesh.normals[j*3];
            float temp2 = this->shape.mesh.normals[j*3+1];
            float temp3 = this->shape.mesh.normals[j*3+2];
            if(temp1 == 0 && temp2 == 2 && temp3)
            {
                vector<float> relativeV = findRelativeVertex(j);
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
                    float v1_position1 = this->shape.mesh.positions[(relativeV[k*3])*3];
                    float v1_position2 = this->shape.mesh.positions[(relativeV[k*3])*3+1];
                    float v1_position3 = this->shape.mesh.positions[(relativeV[k*3])*3+2];

                    float v2_position1 = this->shape.mesh.positions[(relativeV[k*3+1])*3];
                    float v2_position2 = this->shape.mesh.positions[(relativeV[k*3+1])*3+1];
                    float v2_position3 = this->shape.mesh.positions[(relativeV[k*3+1])*3+2];

                    float v3_position1 = this->shape.mesh.positions[(relativeV[k*3+2])*3];
                    float v3_position2 = this->shape.mesh.positions[(relativeV[k*3+2])*3+1];
                    float v3_position3 = this->shape.mesh.positions[(relativeV[k*3+2])*3+2];

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
            else{
                normals[j*3] = this->shape.mesh.normals[j*3];
                normals[j*3+1] = this->shape.mesh.normals[j*3+1];
                normals[j*3+2] = this->shape.mesh.normals[j*3+2];
            }
        }
    }

    if(this->texCoordCount != 0)
    {
        for(int j = 0; j < this->shape.mesh.texcoords.size(); j++){
            tex_coord[tempTCount] = this->shape.mesh.texcoords[j];
            tempTCount++;
        }
    }

    // set up colours
    float *Colours = new float[this->verticeCount*4];
    for(int i = 0; i < this->verticeCount; i++)
    {
        Colours[i*4] = 1.0f;
        Colours[i*4+1] = 1.0f;
        Colours[i*4+2] = 1.0f;
        Colours[i*4+3] = 1.0f;
    }

    //cout <<"In ModelDataVAO:"<<endl;
    //cout <<"tot vertices: "<<this->verticeCount<<endl;
    //cout <<"tot indices: "<<this->indiceCount<<endl;
    //cout <<"tot tex: "<<this->texCoordCount<<" "<<defTex<<endl;

    // The usual code to send data to GPU and link it to shader variables.
    glUseProgram(ProgramID);
    glGenVertexArrays(1, &squareVaoHandle);
    glBindVertexArray(squareVaoHandle);

    int vertLoc = glGetAttribLocation(ProgramID, "a_vertex");
    int colourLoc = glGetAttribLocation(ProgramID, "a_colour");

    //only works in shader 1
    if(checkM == true)
    {
        int texLoc = glGetAttribLocation(ProgramID, "a_tex_coord");
        int normLoc = glGetAttribLocation(ProgramID, "a_normal");

        // buffers
        unsigned int buffer[5];
        glGenBuffers(5, buffer);

        // Set vertex attribute
        glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*this->verticeCount, vertices, GL_STATIC_DRAW);
        // Interpret
        glEnableVertexAttribArray(vertLoc);
        glVertexAttribPointer(vertLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

        // Set colour attr
        glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
        glBufferData(GL_ARRAY_BUFFER,sizeof(float)*4*this->verticeCount, Colours, GL_STATIC_DRAW);
        glEnableVertexAttribArray(colourLoc);
        glVertexAttribPointer(colourLoc, VALS_PER_COLOUR, GL_FLOAT, GL_FALSE, 0, 0);

        // Texture attributes
        if(this->texCoordCount == 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*defTex, tex_coord, GL_STATIC_DRAW);
            glEnableVertexAttribArray(texLoc);
            glVertexAttribPointer(texLoc, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

        }
        else{
            glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*this->texCoordCount, tex_coord, GL_STATIC_DRAW);
            glEnableVertexAttribArray(texLoc);
            glVertexAttribPointer(texLoc, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

        }

        // Normals
        if(this->normalCount == 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*this->verticeCount, normals, GL_STATIC_DRAW);
            glEnableVertexAttribArray(normLoc);
            glVertexAttribPointer(normLoc, VALS_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);
        }
        else{
            glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*this->normalCount, normals, GL_STATIC_DRAW);
            glEnableVertexAttribArray(normLoc);
            glVertexAttribPointer(normLoc, VALS_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);
        }

        // Array of vertex indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[4]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*this->indiceCount, indices, GL_STATIC_DRAW);

        // Un-bind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    else{
        // buffers
        unsigned int buffer[3];
        glGenBuffers(3, buffer);

        // Set vertex attribute
        glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*this->verticeCount, vertices, GL_STATIC_DRAW);
        // Interpret
        glEnableVertexAttribArray(vertLoc);
        glVertexAttribPointer(vertLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

        // Set colour attr
        glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
        glBufferData(GL_ARRAY_BUFFER,sizeof(float)*4*this->verticeCount, Colours, GL_STATIC_DRAW);
        glEnableVertexAttribArray(colourLoc);
        glVertexAttribPointer(colourLoc, VALS_PER_COLOUR, GL_FLOAT, GL_FALSE, 0, 0);

        // Array of vertex indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[2]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*this->indiceCount, indices, GL_STATIC_DRAW);

        // Un-bind
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}


/**
 * @brief modelDataVAO::modelDataVAO
 * @param shapes
 * @param materials
 * @param ProgramID
 * This function is used to create default model data object.
 */

modelDataVAO::modelDataVAO(tinyobj::shape_t shapes, tinyobj::material_t materials, int ProgramID)
{
    //copy all loaded shape data
    //copy indices data
    this->shape.name = shapes.name;
    for(size_t f = 0; f < shapes.mesh.indices.size(); f++){
        unsigned int tempIndice = shapes.mesh.indices[f];
        this->shape.mesh.indices.push_back(tempIndice);
        //cout<< "indices "<<f<<": "<<shapes.mesh.indices[f]<<endl;
    }
    //copy position data
    for(size_t f = 0; f < shapes.mesh.positions.size(); f++){
        float tempPositions = shapes.mesh.positions[f];
        this->shape.mesh.positions.push_back(tempPositions);
        //cout<< "position "<<f<<": "<<shapes.mesh.positions[f]<<endl;
    }
    //copy normals
    for(size_t f = 0; f < shapes.mesh.normals.size(); f++){
        float tempNormals = shapes.mesh.normals[f];
        this->shape.mesh.normals.push_back(tempNormals);
    }
    //copy texcoords
    for(size_t f = 0; f < shapes.mesh.texcoords.size(); f++){
        float tempTexcoords = shapes.mesh.texcoords[f];
        this->shape.mesh.texcoords.push_back(tempTexcoords);
    }
    //copy material_ids
    for(size_t f = 0; f < shapes.mesh.material_ids.size(); f++){
        int tempMaterial_ids = shapes.mesh.material_ids[f];
        this->shape.mesh.material_ids.push_back(tempMaterial_ids);
    }

    this->mat_ID = shapes.mesh.material_ids[0];

    //copy all loaded material data

    this->material.name = materials.name;
    for(size_t f = 0; f < 3; f++){
        this->material.ambient[f] = materials.ambient[f];        
    }
    for(size_t f = 0; f < 3; f++){
        this->material.diffuse[f] = materials.diffuse[f];
    }
    for(size_t f = 0; f < 3; f++){
        this->material.specular[f] = materials.specular[f];
    }
    for(size_t f = 0; f < 3; f++){
        this->material.transmittance[f] = materials.transmittance[f];
    }
    for(size_t f = 0; f < 3; f++){
        this->material.emission[f] = materials.emission[f];
    }
    this->material.shininess = materials.shininess;
    this->material.ior = materials.ior;
    this->material.dissolve = materials.dissolve;
    this->material.illum = materials.illum;
    this->material.ambient_texname = materials.ambient_texname;
    this->material.diffuse_texname = materials.diffuse_texname;
    this->material.specular_texname = materials.specular_texname;
    this->material.normal_texname = materials.normal_texname;
}



/**
 * @brief modelDataVAO::render
 * @param ProgramID
 * @param mvMatrix
 * @param checkM
 * @param mode
 * @param position
 * This function is used to render its own shape.
 */

void modelDataVAO::render(int ProgramID, glm::mat4 mvMatrix, bool checkM, int mode, glm::vec3 position, float rotAngle, glm::vec3 rotAxis)
{
    glUseProgram(ProgramID);

    int modelUniformHandle = glGetUniformLocation(ProgramID, "model_matrix");

    //only works in shader 1 mode
    if(checkM == true)
    {
        // update SHININESS
        GLfloat shininess[1];
        shininess[0] = this->material.shininess;    
        int shin = glGetUniformLocation(ProgramID,"shininess");
        if (shin == -1) {
            fprintf(stderr, "Error: can't find shininess\n");
            exit(1);
        }
        glUniform1fv(shin, 1, shininess);

        // update current light mode
        GLfloat currentLightMode[1];
        currentLightMode[0] = mode;
        int light_mode = glGetUniformLocation(ProgramID,"light_mode");
        if (light_mode == -1) {
            fprintf(stderr, "Error: can't find lightMode\n");
            exit(1);
        }
        glUniform1fv(light_mode, 1, currentLightMode);

        // Uniform variables defining material colours
        // These can be changed for each sphere, to compare effects
        int mtlambientHandle = glGetUniformLocation(ProgramID, "mtl_ambient");
        int mtldiffuseHandle = glGetUniformLocation(ProgramID, "mtl_diffuse");
        int mtlspecularHandle = glGetUniformLocation(ProgramID, "mtl_specular");

        // setup mtl color
        float mtlambient[3];
        float mtldiffuse[3];
        float mtlspecular[3];
        for(int i = 0; i < 3; i++)
        {
            mtlambient[i] = material.ambient[i];
            mtldiffuse[i] = material.diffuse[i];
            mtlspecular[i] = material.specular[i];           
        }

        glUniform3fv(mtlambientHandle, 1, mtlambient);
        glUniform3fv(mtldiffuseHandle, 1, mtldiffuse);
        glUniform3fv(mtlspecularHandle, 1, mtlspecular);

        glBindVertexArray(squareVaoHandle);

        // scaling
        glm::mat4 model = glm::mat4(1.0);
        model = glm::rotate(model, rotAngle, rotAxis);
        model = glm::translate(model, position);        
        glUniformMatrix4fv(modelUniformHandle, 1, false, glm::value_ptr(model));
    }
    else{
        glBindVertexArray(squareVaoHandle);

        // scaling
        glm::mat4 model = glm::mat4(1.0);
        model = glm::rotate(model, rotAngle, rotAxis);
        model = glm::translate(model, position);
        glUniformMatrix4fv(modelUniformHandle, 1, false, glm::value_ptr(model));
    }
    // Draw the square
    glDrawElements(GL_TRIANGLES, this->indiceCount, GL_UNSIGNED_INT, 0);
}

/**
 * @brief render
 * @param ProgramID
 * @param fs
 * @param mvMatrix
 * @param checkM
 * @param mode
 * @param scaleVec
 * @param position
 * This function is used to render the model and set its position.
 */
glm::mat4 modelDataVAO::render(int ProgramID,float fs, glm::mat4 mvMatrix, bool checkM, int mode, std::vector<float> scaleVec, glm::vec3 position, std::vector<float> rotAngle, std::vector<glm::vec3> rotAxis)
{

    glUseProgram(ProgramID);

    int modelUniformHandle = glGetUniformLocation(ProgramID, "model_matrix");
    if (modelUniformHandle == -1) {
        fprintf(stderr, "Could not find model View\n");
        exit(1);
    }

    //only works in shader 1 mode
    glm::mat4 objModel;
    if(checkM == true)
    {
        // update SHININESS
        GLfloat shininess[1];
        shininess[0] = this->material.shininess;
        int shin = glGetUniformLocation(ProgramID,"shininess");
        if (shin == -1) {
            fprintf(stderr, "Error: can't find shininess\n");
            exit(1);
        }
        glUniform1fv(shin, 1, shininess);

        // update current light mode
        GLfloat currentLightMode[1];
        currentLightMode[0] = mode;
        int light_mode = glGetUniformLocation(ProgramID,"light_mode");
        if (light_mode == -1) {
            fprintf(stderr, "Error: can't find lightMode\n");
            exit(1);
        }
        glUniform1fv(light_mode, 1, currentLightMode);



        // Uniform variables defining material colours
        // These can be changed for each sphere, to compare effects
        int mtlambientHandle = glGetUniformLocation(ProgramID, "mtl_ambient");
        int mtldiffuseHandle = glGetUniformLocation(ProgramID, "mtl_diffuse");
        int mtlspecularHandle = glGetUniformLocation(ProgramID, "mtl_specular");

        // setup mtl color
        float mtlambient[3];
        float mtldiffuse[3];
        float mtlspecular[3];
        for(int i = 0; i < 3; i++)
        {
            mtlambient[i] = material.ambient[i];
            mtldiffuse[i] = material.diffuse[i];
            mtlspecular[i] = material.specular[i];
        }
        glUniform3fv(mtlambientHandle, 1, mtlambient);
        glUniform3fv(mtldiffuseHandle, 1, mtldiffuse);
        glUniform3fv(mtlspecularHandle, 1, mtlspecular);

        glBindVertexArray(squareVaoHandle);

        // scaling
        glm::mat4 model = glm::mat4(1.0);
        model = glm::scale(model, glm::vec3(0.3/fs,0.3/fs,0.3/fs));
        for(int i = 0; i < rotAngle.size(); i++)
        {
            model = glm::rotate(model, rotAngle[i], rotAxis[i]);
        }
        model = glm::translate(model, position);     
        glUniformMatrix4fv(modelUniformHandle, 1, false, glm::value_ptr(model));
        objModel = glm::mat4(model);
    }
    else{
        glBindVertexArray(squareVaoHandle);

        // scaling
        glm::mat4 model = glm::mat4(1.0);
        model = glm::scale(model, glm::vec3(0.3/fs,0.3/fs,0.3/fs));
        for(int i = 0; i < rotAngle.size(); i++)
        {
            model = glm::rotate(model, rotAngle[i], rotAxis[i]);
        }
        model = glm::translate(model, position);

        glUniformMatrix4fv(modelUniformHandle, 1, false, glm::value_ptr(model));
        objModel = glm::mat4(model);
    }
    // Draw the square
    glDrawElements(GL_TRIANGLES, this->indiceCount, GL_UNSIGNED_INT, 0);

    return objModel;
}

/**
 * @brief modelDataVAO::modelSize
 * @return vector<float> max_min
 * This function is used to calculate the max/min value of model in x, y and z.
 */

std::vector<float> modelDataVAO::modelSize()
{
    //initialize
    //x_max, x_min, y_max, y_min, z_max, z_min
    vector<float> result;
    for(int i = 0; i < 6; i++){
        float temp = 0;
        result.push_back(temp);
    }

    //check max and min value

    for(int j = 0; j < this->shape.mesh.positions.size()/3; j++)
    {
        //check max and min X
        if(this->shape.mesh.positions[j*3] >= result[0])
        {
            result[0] = this->shape.mesh.positions[j*3];
        }
        else if(this->shape.mesh.positions[j*3] < result[1])
        {
            result[1] = this->shape.mesh.positions[j*3];
        }
        //check max and min Y
        if(this->shape.mesh.positions[j*3+1] >= result[2])
        {
            result[2] = this->shape.mesh.positions[j*3+1];
        }
        else if(this->shape.mesh.positions[j*3+1] < result[3])
        {
            result[3] = this->shape.mesh.positions[j*3+1];
        }
        //check max and min Z
        if(this->shape.mesh.positions[j*3+2] >= result[4])
        {
            result[4] = this->shape.mesh.positions[j*3+2];
        }
        else if(this->shape.mesh.positions[j*3+2] < result[5])
        {
            result[5] = this->shape.mesh.positions[j*3+2];
        }
    }
    return result;
}

/**
 * @brief modelDataVAO::getIndiceCount
 * @return int
 * This function is used to return # of indice.
 */

int modelDataVAO::getIndiceCount()
{
    return this->indiceCount;
}

/**
 * @brief modelDataVAO::getVerticeCount
 * @return int
 * This function is used to return # of vertex.
 */
int modelDataVAO::getVerticeCount()
{
    return this->verticeCount;
}

/**
 * @brief modelDataVAO::getNumOfTex
 * @return int
 * This function is used to return # of texture.
 */
int modelDataVAO::getNumOfTex()
{
    return this->NumOfTex;
}

/**
 * @brief modelDataVAO::getVaoHandle
 * @return int
 * This function is used to return VAO handler.
 */
unsigned int modelDataVAO::getVaoHandle()
{
    return squareVaoHandle;
}

/**
 * @brief modelDataVAO::getMatID
 * @return int
 * This function is used to return material_ids.
 */
int modelDataVAO::getMatID()
{
    return this->mat_ID;
}

/**
 * @brief modelDataVAO::getNormalCount
 * @return int
 * This function is used to return # of normal.
 */
int modelDataVAO::getNormalCount()
{
    return this->normalCount;
}

/**
 * @brief modelDataVAO::autoCalNormals
 * @param v1
 * @param v2
 * @param v3
 * @return vector<float>
 * This function is used to auto-generate normals.
 */
std::vector<float> modelDataVAO::autoCalNormals(std::vector<float> v1, std::vector<float> v2, std::vector<float> v3)
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
std::vector<float> modelDataVAO::findRelativeVertex(int index)
{
    vector<float> relativeIndices;
    for(int i = 0; i < this->indiceCount/3; i++)
    {
        if(this->shape.mesh.indices[i*3] == index
                || this->shape.mesh.indices[i*3+1] == index
                || this->shape.mesh.indices[i*3+2] == index)
        {
            float v1 = this->shape.mesh.indices[i*3];
            float v2 = this->shape.mesh.indices[i*3+1];
            float v3 = this->shape.mesh.indices[i*3+2];
            relativeIndices.push_back(v1);
            relativeIndices.push_back(v2);
            relativeIndices.push_back(v3);
        }
    }
    return relativeIndices;
}
