//Author: pz.yao
//Date: 18 May 2015
//Program description:
//CG Assignment 3 SKY GLASS KITCHEN

#include "terrain.h"
#include <iostream>
#include <stdlib.h>
#include <iomanip>
#include <stdio.h>
#include <fstream>
#include <string>
#include <time.h>
#include <vector>
#include <cstdlib>
#include "tiny_obj_loader.h"

#include <GL/glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Viewer.h"

#define VALS_PER_VERT 3
#define VALS_PER_COLOUR 4
#define VALS_PER_TEX 2
#define VALS_PER_NORMAL 3

using namespace std;

/**
 * @brief Terrian::Terrain
 * This function is used to read map data from text file.
 */
Terrain::Terrain()
{
    string fileName = "terrain.csv";
    ifstream infile(fileName.c_str()); //open the file
    if (infile.is_open() && infile.good()) {     
        infile>>mapSize;
    }
    else {
        cout << "Can't open the terrian.csv file.\n";
    }

    //setup all attributes for vertex drawing
    totNumPoints = mapSize*mapSize;//read from file
    indexNum = totNumPoints*3;
    sideNodes = mapSize;//read from file
    float width = 2.0f/(mapSize-1.0f);
    nodes = new float[indexNum];

    //set x-axis location of all nodes
    int x_count = 0;
    for(int i = 0; i <indexNum;i = i+3)
    {
            if(x_count == 0)
            {
                nodes[i] = -1.0f;
                x_count++;
            }
            else
            {
                nodes[i] = nodes[i-3]+width;
                x_count ++;
                if(x_count == mapSize)
                {
                    x_count = 0;
                }
            }
    }

    // set Z-axis location of all nodes
    int y_count = 0;
    float length  = -1.0f;
    for(int i = 2; i < indexNum; i = i+3)
    {
          nodes[i]=length;
          y_count++;
          if(y_count == mapSize)
          {
            length = length+width;
            y_count = 0;
          }
    }

    // set Y-location read from text file
    ifstream infile2(fileName.c_str()); //open the file
    if (infile2.is_open() && infile2.good()) {       
        float tempValue;
        int index_z = 1;
        bool order = false;
        while (infile2 >> tempValue)
        {
            if(order == true)
            {
                nodes[index_z] = tempValue;
                index_z = index_z +3;
            }
            else
            {
                order = true;
            }
        }
    }
    else {
        cout << "Can't open the terrian.csv file.\n";
    }
}


/**
 * @brief Terrain::createTerrianVAO
 * @param ProgramID
 * @param checkM
 * @return
 * This function is used to create VAO per shape, and containing the
 * auto-generate normal function.
 */
int Terrain::createTerrianVAO(unsigned int ProgramID, bool checkM)
{
    Vcount = indexNum;
    Ccount = 0;
    Tcount = 0;
    Ncount = 0;
    Icount = 0;

    // Set colours for each vertex; red, green, blue and alpha
    // Red, Green, Blue, Dark?
    float* colours = new float[totNumPoints*4];
    for(int i = 0; i < totNumPoints*4; i=i+4)
    {
        colours[i] = 1.0f;
        colours[i+1] = 1.0f;
        colours[i+2] = 1.0;
        colours[i+3] = 1.0f;
        Ccount += 4;
    }

    //setup indices properties
    //setup the connect order of each triangles
    totNumOfIndices = (sideNodes-1)*(sideNodes-1)*2*3;

    indices = new int[totNumOfIndices];
    int index_indices = 0;
    int index_check = 0;
    for(int i = 0; i < totNumOfIndices ;i = i+6)
    {
        //connecting order
        //line up:[n,n+1,n+w]
        //line down:[n+1,n+w,n+w+1]
        indices[i] = index_indices;
        indices[i+1] = index_indices+1;
        indices[i+2] = index_indices+sideNodes;
        indices[i+3] = index_indices+1;
        indices[i+4] = index_indices+sideNodes;
        indices[i+5] = index_indices+sideNodes+1;
        index_indices++;
        index_check++;
        if(index_check == sideNodes - 1)
        {
            index_check = 0;
            index_indices++;
        }
        Icount += 6;
    }

    // tex coord
    int numOfTex = totNumPoints*2;
    texCoord = new float[numOfTex];
    float texWidth = 1.0f/(sideNodes-1);
    float horizontal = 0.0f;
    float vertical = 0.0f;
    for(int i = 0; i < numOfTex; i = i + 2)
    {
        if(horizontal > 1.0f)
        {
            horizontal = 0.0f;
            vertical += texWidth;
        }
        texCoord[i] = horizontal;
        texCoord[i+1] = vertical;

        horizontal += texWidth;
        Tcount += 2;
    }


    // Normals 
    // check to see if there exists normals
    // if not, auto-generate normals per vertex

    normals = new float[this->indexNum];
    for(int j = 0; j < this->indexNum/3; j++)
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
            float v1_position1 = this->nodes[(relativeV[k*3])*3];
            float v1_position2 = this->nodes[(relativeV[k*3])*3+1];
            float v1_position3 = this->nodes[(relativeV[k*3])*3+2];

            float v2_position1 = this->nodes[(relativeV[k*3+1])*3];
            float v2_position2 = this->nodes[(relativeV[k*3+1])*3+1];
            float v2_position3 = this->nodes[(relativeV[k*3+1])*3+2];

            float v3_position1 = this->nodes[(relativeV[k*3+2])*3];
            float v3_position2 = this->nodes[(relativeV[k*3+2])*3+1];
            float v3_position3 = this->nodes[(relativeV[k*3+2])*3+2];

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
                sumNormal[0] = 0.0f;
                sumNormal[1] = -1.0f;
                sumNormal[2] = 0.0f;
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

        Ncount += 3;     
    }
    normals[3*3+1] = -1;




    // Generate storage on the GPU for our triangle and make it current.
    // The usual code to send data to GPU and link it to shader variables.
    glUseProgram(ProgramID);
    glGenVertexArrays(1, &terrainVaoHandle);
    glBindVertexArray(terrainVaoHandle);

    // Generate new buffers in our VAO
    int vertLoc = glGetAttribLocation(ProgramID, "a_vertex");
    int colourLoc = glGetAttribLocation(ProgramID, "a_colour");

    if(checkM == true)
    {
        int texLoc = glGetAttribLocation(ProgramID, "a_tex_coord");
        int normLoc = glGetAttribLocation(ProgramID, "a_normal");

        // buffers
        unsigned int buffer[5];
        glGenBuffers(5, buffer);


        // Allocate GPU memory for our vertices and copy them over
        glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * Vcount, nodes, GL_STATIC_DRAW);
        glEnableVertexAttribArray(vertLoc);
        glVertexAttribPointer(vertLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

        // Do the same for our vertex colours
        glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * Ccount, colours, GL_STATIC_DRAW);
        glEnableVertexAttribArray(colourLoc);
        glVertexAttribPointer(colourLoc, VALS_PER_COLOUR, GL_FLOAT, GL_FALSE, 0, 0);

        // Texture attributes
        glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * Tcount, texCoord, GL_STATIC_DRAW);
        glEnableVertexAttribArray(texLoc);
        glVertexAttribPointer(texLoc, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

        // Normals
        glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * Ncount, normals, GL_STATIC_DRAW);
        glEnableVertexAttribArray(normLoc);
        glVertexAttribPointer(normLoc, VALS_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);

        // Array of vertex indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[4]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * Icount, indices,GL_STATIC_DRAW);

        // Un-bind
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }
    else{
        // A single data buffer store for generic, per-vertex attributes
        unsigned int buffer[3];
        glGenBuffers(3, buffer);

        // Allocate GPU memory for our vertices and copy them over
        glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * Vcount, nodes, GL_STATIC_DRAW);
        glEnableVertexAttribArray(vertLoc);
        glVertexAttribPointer(vertLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

        // Do the same for our vertex colours
        glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * Ccount, colours, GL_STATIC_DRAW);
        glEnableVertexAttribArray(colourLoc);
        glVertexAttribPointer(colourLoc, VALS_PER_COLOUR, GL_FLOAT, GL_FALSE, 0, 0);

        // allocate indices memory
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[2]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * Icount, indices,GL_STATIC_DRAW);

        // Un-bind
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    return 0;	// return success
}

/**
 * @brief Terrain::render
 * @param ProgramID
 * @param fs
 * @param mvMatrix
 * @param checkM
 * @param mode
 * @param scaleVec
 * @param position
 * This function is used to render the model and set its position.
 */
void Terrain::render(unsigned int ProgramID, float fs, bool checkM, int mode, std::vector<float> scaleVec, glm::vec3 position, float rotAngle, glm::vec3 rotAxis, float shin){

    glUseProgram(ProgramID);


    int modelUniformHandle = glGetUniformLocation(ProgramID, "model_matrix");


    //only works in shader 1 mode
    if(checkM == true)
    {      
        // update current light mode
        GLfloat currentLightMode[1];
        currentLightMode[0] = mode;
        int light_mode = glGetUniformLocation(ProgramID,"light_mode");
        if (light_mode == -1) {
            fprintf(stderr, "Error: can't find lightMode\n");
            exit(1);
        }
        glUniform1fv(light_mode, 1, currentLightMode);
        // update SHININESS
        GLfloat shininess[1];
        shininess[0] = shin;
        int shin = glGetUniformLocation(ProgramID,"shininess");
        if (shin == -1) {
            fprintf(stderr, "Error: can't find shininess\n");
            exit(1);
        }
        glUniform1fv(shin, 1, shininess);

        // Uniform variables defining material colours
        // These can be changed for each sphere, to compare effects
        int mtlambientHandle = glGetUniformLocation(ProgramID, "mtl_ambient");
        int mtldiffuseHandle = glGetUniformLocation(ProgramID, "mtl_diffuse");
        int mtlspecularHandle = glGetUniformLocation(ProgramID, "mtl_specular");

        // setup mtl color
        float mtlambient[3];
        float mtldiffuse[3];
        float mtlspecular[3];

        mtlambient[0] = 0.12f;
        mtlambient[1] = 0.12f;
        mtlambient[2] = 0.12f;

        mtldiffuse[0] = 1.0f;
        mtldiffuse[1] = 1.0f;
        mtldiffuse[2] = 1.0f;

        mtlspecular[0] = 1.0f;
        mtlspecular[1] = 1.0f;
        mtlspecular[2] = 1.0f;

        glUniform3fv(mtlambientHandle, 1, mtlambient);
        glUniform3fv(mtldiffuseHandle, 1, mtldiffuse);
        glUniform3fv(mtlspecularHandle, 1, mtlspecular);

        glBindVertexArray(terrainVaoHandle);
        // scaling
        glm::mat4 model = glm::mat4(1.0);
        model = glm::scale(model,glm::vec3(fs,fs,fs));
        model = glm::rotate(model, rotAngle, rotAxis);
        model = glm::translate(model, position);
        glUniformMatrix4fv(modelUniformHandle, 1, false, glm::value_ptr(model));
    }
    else{
        glBindVertexArray(terrainVaoHandle);

        // scaling
        glm::mat4 model = glm::mat4(1.0);
        model = glm::scale(model, glm::vec3(fs,fs,fs));
        model = glm::rotate(model, rotAngle, rotAxis);
        model = glm::translate(model, position);
        glUniformMatrix4fv(modelUniformHandle, 1, false, glm::value_ptr(model));
    }
    // Draw the square
    glDrawElements(GL_TRIANGLES, this->totNumOfIndices, GL_UNSIGNED_INT, 0);

}

/**
 * @brief Terrain::autoCalNormals
 * @param v1
 * @param v2
 * @param v3
 * @return vector<float>
 * This function is used to auto-generate normals.
 */
std::vector<float> Terrain::autoCalNormals(std::vector<float> v1, std::vector<float> v2, std::vector<float> v3)
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
 * @brief Terrain::findRelativeVertex
 * @param index
 * @return vector<float>
 * This function is used to find surface of shared vertex.
 */
std::vector<int> Terrain::findRelativeVertex(int index)
{
    vector<int> relativeIndices;

    for(int i = 0; i < this->totNumOfIndices/3; i++)
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


unsigned int Terrain::getTerrainVAOHandle(){
    return terrainVaoHandle;
}

int Terrain::getTotNumOfIndices(){
    return totNumOfIndices;
}
