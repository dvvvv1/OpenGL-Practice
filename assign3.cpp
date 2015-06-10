//Author: Puzhi Yao
//Date: 18 May 2015
//Program description:
//CG Assignment 3 SKY GLASS KITCHEN

#include <iostream>
#include <stdlib.h>
#include <iomanip>
#include <stdio.h>
#include <fstream>
#include <string>
#include <time.h>
#include <vector>

#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "tiny_obj_loader.h"
#include "Viewer.h"
#include "InputState.h"
#include "Shader.hpp"
#include "modelDataVAO.h"
#include "terrain.h"
#include "lightSource.h"
#include "Sphere.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TEX_SIZE        64

#define VALS_PER_VERT 3
#define VALS_PER_COLOUR 4
#define VALS_PER_TEX 2          // number of dimensions of the texture data (if image, set it to 2)
#define POLYGON_NUM_TRIS 2      // number of squares in a cube
#define POLYGON_NUM_VERTICES 4     // number of vertices in a cube`
#define VALS_PER_TEXTURE 2

// Globe variable
InputState Input;
bool checkMode = true;
int NumOfTex = 0;

Sphere *sphere;
unsigned int sphereVaoHandle;
// Camera
ObjectViewer *ObjCam;
ThirdPersonViewer *TPCam;
Viewer *Camera;
int camMode = 1;
glm::vec3 robotPos;
glm::vec3 eye;
glm::vec3 at;
glm::vec3 up;
glm::mat4 projection;
glm::mat4 viewMatrix;
float wave;

// texture image
GLubyte myimage[TEX_SIZE][TEX_SIZE][4];

// Program Handle & Shaders
unsigned int axisVaoHandle;
unsigned int skyBoxVaoHandle;
unsigned int ProgramID; // texutre & light
unsigned int ProgramID2;// wireframe
unsigned int ProgramID3;// bump mapping
unsigned int ProgramID4;// bump mapping sky -->not using
unsigned int ProgramID5;// skyBox
unsigned int ProgramID6;// mirror shade -->not using
std::vector<unsigned int> ProgramIDVec;

// all objects
std::vector< std::vector<tinyobj::shape_t> > allModel;
std::vector< std::vector<tinyobj::material_t> > allMTL;

// 1st model character maya
std::vector<tinyobj::shape_t> shapes;
std::vector<tinyobj::material_t> materials;
std::vector<modelDataVAO*> modelVector;

// 2nd model building garden
std::vector<tinyobj::shape_t> shapes2;
std::vector<tinyobj::material_t> materials2;
std::vector<modelDataVAO*> modelVector2;

// 3rd model character zero
std::vector<tinyobj::shape_t> shapes3;
std::vector<tinyobj::material_t> materials3;
std::vector<modelDataVAO*> modelVector3;

// 4th model robot R2
std::vector<tinyobj::shape_t> shapes4;
std::vector<tinyobj::material_t> materials4;
std::vector<modelDataVAO*> modelVector4;

// 4.5th model robot R2 mirror part
std::vector<modelDataVAO*> modelVector4MIRROR;

// 5th model inner room
std::vector<tinyobj::shape_t> shapes5;
std::vector<tinyobj::material_t> materials5;
std::vector<modelDataVAO*> modelVector5;

// 6th model light bulb cube 1
std::vector<tinyobj::shape_t> shapes6;
std::vector<tinyobj::material_t> materials6;
std::vector<modelDataVAO*> modelVector6;

// 7th model mirror
std::vector<tinyobj::shape_t> shapes7;
std::vector<tinyobj::material_t> materials7;
std::vector<modelDataVAO*> modelVector7;

// other globe variables
float lightForward;
float lightLeft;
float lightup;
int movePress;
float moveForward;
float moveLeft;

// terrian plains
Terrain *glassPlain;
Terrain *ovenPlain;
Terrain *stovePlain;
Terrain *plain;
Terrain *plain2;
modelDataVAO *shapeData;

// lights
lightSource *bulb;
lightSource *bulb2;
lightSource *bulb3;
std::vector<float> scales;
std::string imagePath;
int lightMode = 0;
int lightingEffectSwitch[1];
std::vector<float> tempScaleVec;

/**
 Variables defining how scene is drawn
*/
static float xRot = 0.0f;
static float yRot = 0.0f;

static double FOV = 45.0;
//static int winW = 640;
//static int winH = 480;
int winW;
int winH;
/**
 Variables defining texture settings
*/
static GLenum CurrentTexSample = GL_NEAREST;
static char   CurrentTexString[32] = "GL_NEAREST";

static GLenum CurrentTexMode = GL_REPLACE;
static char   CurrentTexModeString[32] = "GL_REPLACE";
GLuint *TexID;
GLuint *TexID2;
GLuint TexID3;
static GLuint CurrentTex = 0;

using namespace std;

/**
 * @brief createSphereVAO
 * @param currentProgram
 * @param checkM
 * @return
 * This function is used to create a sphere VAO.
 */
int createSphereVAO(unsigned int currentProgram,bool checkM)
{
    if (!sphere) {
        fprintf(stderr, "Error: initalise sphere before creating VAO!\n");
    }

    glGenVertexArrays(1, &sphereVaoHandle);
    glBindVertexArray(sphereVaoHandle);


    int vertLoc = glGetAttribLocation(currentProgram, "a_vertex");
    int colourLoc = glGetAttribLocation(currentProgram, "a_colour");

    //only works in shader 1
    if(checkM == true)
    {
        int normLoc = glGetAttribLocation(currentProgram, "a_normal");
        int texCoordLoc = glGetAttribLocation(currentProgram, "a_tex_coord");

        unsigned int buffer[5];
        glGenBuffers(5, buffer);

        // Set vertex attribute
        glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*sphere->vertCount, sphere->vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(vertLoc);
        glVertexAttribPointer(vertLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

        // Set colour attr
        glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
        glBufferData(GL_ARRAY_BUFFER,sizeof(float)*sphere->colorCount, sphere->colours, GL_STATIC_DRAW);
        glEnableVertexAttribArray(colourLoc);
        glVertexAttribPointer(colourLoc, VALS_PER_COLOUR, GL_FLOAT, GL_FALSE, 0, 0);

        //Texture
        glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*sphere->texCount, sphere->texCoords, GL_STATIC_DRAW);
        glEnableVertexAttribArray(texCoordLoc);
        glVertexAttribPointer(texCoordLoc, VALS_PER_TEXTURE, GL_FLOAT, GL_FALSE, 0, 0);


        // Normal attributes
        glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*sphere->normCount, sphere->normals, GL_STATIC_DRAW);
        glEnableVertexAttribArray(normLoc);
        glVertexAttribPointer(normLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

        // Index
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[4]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*sphere->indCount, sphere->indices, GL_STATIC_DRAW);

        // Un-bind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    else
    {
        // buffers
        unsigned int buffer[3];
        glGenBuffers(3, buffer);

        // Set vertex attribute
        glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*sphere->vertCount, sphere->vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(vertLoc);
        glVertexAttribPointer(vertLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

        // Set colour attr
        glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
        glBufferData(GL_ARRAY_BUFFER,sizeof(float)*sphere->colorCount, sphere->colours, GL_STATIC_DRAW);
        glEnableVertexAttribArray(colourLoc);
        glVertexAttribPointer(colourLoc, VALS_PER_COLOUR, GL_FLOAT, GL_FALSE, 0, 0);

        // Index
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[2]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*sphere->indCount, sphere->indices, GL_STATIC_DRAW);

        // Un-bind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    return 0; // success

}


/**
 * @brief useTinyOBJLoader
 * This function is used for loading .obj data and storing those
 * data into globle data variable.
 */
void useTinyOBJLoader()
{
    // setup obj/mtl file location
    std::cout <<"Loading obj file....."<<std::endl;
    std::string inputOBJ1 = "obj/Human/Maya/maya.obj";
    std::string inputMat1 = "obj/Human/Maya/";
    imagePath = "obj/picture/";

    std::string inputOBJ2 = "obj/mirror/mirror.obj";
    std::string inputMat2 = "obj/mirror/";
    imagePath = "obj/picture/";

    std::string inputOBJ3 = "obj/Human/Zero/zero.obj";
    std::string inputMat3 = "obj/Human/Zero/";
    imagePath = "obj/picture/";

    std::string inputOBJ4 = "obj/Robot/R2/R2.obj";
    std::string inputMat4 = "obj/Robot/R2/";
    imagePath = "obj/picture/";

    std::string inputOBJ5 = "obj/texRoom/roomAll.obj";
    std::string inputMat5 = "obj/texRoom/";
    imagePath = "obj/picture/";

    std::string inputOBJ6 = "obj/cube-tex/cube-tex.obj";
    std::string inputMat6 = "obj/cube-tex/";
    imagePath = "obj/picture/";

    std::string inputOBJ7 = "obj/missile/missile2.obj";
    std::string inputMat7 = "obj/missile/";
    imagePath = "obj/picture/";

    // load obj/mtl file
    std::string err;
    std::string err2;
    std::string err3;
    std::string err4;
    std::string err5;
    std::string err6;
    std::string err7;

    // loading object data from obj file and mtl.
    err = tinyobj::LoadObj(shapes, materials, inputOBJ1.c_str(),inputMat1.c_str());
    err2 = tinyobj::LoadObj(shapes2, materials2, inputOBJ2.c_str(),inputMat2.c_str());
    err3 = tinyobj::LoadObj(shapes3, materials3, inputOBJ3.c_str(),inputMat3.c_str());
    err4 = tinyobj::LoadObj(shapes4, materials4, inputOBJ4.c_str(),inputMat4.c_str());
    err5 = tinyobj::LoadObj(shapes5, materials5, inputOBJ5.c_str(),inputMat5.c_str());
    err6 = tinyobj::LoadObj(shapes6, materials6, inputOBJ6.c_str(),inputMat6.c_str());
    err7 = tinyobj::LoadObj(shapes7, materials7, inputOBJ7.c_str(),inputMat7.c_str());

    allModel.push_back(shapes);
    allModel.push_back(shapes2);
    allModel.push_back(shapes3);
    allModel.push_back(shapes4);
    allModel.push_back(shapes5);
    allModel.push_back(shapes6);
    allModel.push_back(shapes7);

    allMTL.push_back(materials);
    allMTL.push_back(materials2);
    allMTL.push_back(materials3);
    allMTL.push_back(materials4);
    allMTL.push_back(materials5);
    allMTL.push_back(materials6);
    allMTL.push_back(materials7);

    std::cout<<"OBJ File path checking success. "<<std::endl;
    std::cout<<"Loading data...."<<std::endl;

    // count for # of tot tex
    for(int i = 0 ; i < allMTL.size(); i++)
    {
        NumOfTex+=allMTL[i].size();
    }
    NumOfTex += 6;
    cout<<"Total number of textures need to load: "<<NumOfTex<<endl;
}

/**
 * @brief createAxis
 * @return int
 * This function is used to create world axis VAO data.
 */
int createAxis()
{
    //This function is uesd to draw world axis
    // Generating object
    // vertices define
    float vertices [] = {1.0f,0.0f,0.0f, //x axis
                         -1.0f,0.0f,0.0f,

                          1.0f,0.0f,0.0f,// x arrow
                         0.95f,0.05f,0.0f,
                          1.0f,0.0f,0.0f,
                         0.95f,-0.05f,0.0f,

                         0.0f,1.0f,0.0f,// y axis
                         0.0f,-1.0f,0.0f,

                         0.0f,1.0f,0.0f,// y arrow
                         0.0f,0.95f,0.05f,
                         0.0f,1.0f,0.0f,
                         0.0f,0.95f,-0.05f,


                        0.0f,0.0f,1.0f,// z axis
                        0.0f,0.0f,-1.0f,

                        0.0f,0.0f,1.0f,// z arrow
                        0.05f,0.0f,0.95f,
                        0.0f,0.0f,1.0f,
                        -0.05f,0.0f,0.95f};

    // Define the single colour for world axis
    float colours [] = {0.0f,0.0f,1.0f,1.0f,
                        0.0f,0.0f,1.0f,1.0f,
                        0.0f,0.0f,1.0f,1.0f,
                        0.0f,0.0f,1.0f,1.0f,
                        0.0f,0.0f,1.0f,1.0f,
                        0.0f,0.0f,1.0f,1.0f,

                       0.0f,1.0f,0.0f,1.0f,
                       0.0f,1.0f,0.0f,1.0f,
                        0.0f,1.0f,0.0f,1.0f,
                        0.0f,1.0f,0.0f,1.0f,
                        0.0f,1.0f,0.0f,1.0f,
                        0.0f,1.0f,0.0f,1.0f,

                       1.0f,0.5f,0.0f,1.0f,
                       1.0f,0.5f,0.0f,1.0f,
                        1.0f,0.5f,0.0f,1.0f,
                        1.0f,0.5f,0.0f,1.0f,
                        1.0f,0.5f,0.0f,1.0f,
                        1.0f,0.5f,0.0f,1.0f,};


    // The usual code to send data to GPU and link it to shader variables.
    glGenVertexArrays(1, &axisVaoHandle);
    glBindVertexArray(axisVaoHandle);

    int vertLoc2 = glGetAttribLocation(ProgramID2, "a_vertex");
    int colourLoc2 = glGetAttribLocation(ProgramID2, "a_colour");

    // buffers
    unsigned int buffer[3];
    glGenBuffers(3, buffer);

    // Set vertex attribute
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Interpret
    glEnableVertexAttribArray(vertLoc2);
    glVertexAttribPointer(vertLoc2, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

    // Set colour attr
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(colours), colours, GL_STATIC_DRAW);
    glEnableVertexAttribArray(colourLoc2);
    glVertexAttribPointer(colourLoc2, VALS_PER_COLOUR, GL_FLOAT, GL_FALSE, 0, 0);

    // Un-bind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return 0; // success
}

/**
 * @brief createCube
 * @return
 * This function is used to create sky box Cube.
 */
int createCube()
{
    float CubeVertex [] ={
        -1.0,1.0,1.0,
        -1.0,-1.0,1.0,
        1.0,-1.0,1.0,
        1.0,1.0,1.0,

        -1.0,1.0,-1.0,
        -1.0,-1.0,-1.0,
        1.0,-1.0,-1.0,
        1.0,1.0,-1.0
    };

    unsigned int CubeIndice [] = {
        0,1,2, //front
        0,3,2,

        2,3,6, // left
        3,7,6,

        4,5,6, // back
        4,7,6,

        0,1,5, //right
        0,4,5,

        0,3,4, // top
        3,4,7,

        1,2,6, // bottom
        1,5,6
    };

    // The usual code to send data to GPU and link it to shader variables.
    glGenVertexArrays(1, &skyBoxVaoHandle);
    glBindVertexArray(skyBoxVaoHandle);

    int cubeVertLoc = glGetAttribLocation(ProgramID5, "a_vertex");

    // buffers
    unsigned int buffer[2];
    glGenBuffers(2, buffer);

    // Set vertex attribute
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertex), CubeVertex, GL_STATIC_DRAW);
    // Interpret
    glEnableVertexAttribArray(cubeVertLoc);
    glVertexAttribPointer(cubeVertLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

    // Array of vertex indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CubeIndice), CubeIndice, GL_STATIC_DRAW);

    // Un-bind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return 0; // success
}


/**
 * @brief createTextures
 * @return int
 * This function is used to create default white texture
 * and store image data into array of texture.
 * This function has used 3 texture buffers: texture0, texuture1 and texture2.
 */
GLuint createTextures()
{
    // -------------------------------------------Create White Board----------------------------------------------------------------------
    TexID = new GLuint[NumOfTex];
    // A shader program has many texture units, slots in which a texture can be bound, available to
	// it and this function defines which unit we are working with currently
	// We will only use unit 0 until later in the course. This is the default.
    glActiveTexture(GL_TEXTURE0);

    glGenTextures( NumOfTex, TexID );

    // create white board
    for(int i=0; i < TEX_SIZE; i++){
        for(int j=0; j < TEX_SIZE; j++)
        {
            GLubyte c;
            c = 255;
            myimage[i][j][0]  = (GLubyte)c;
            myimage[i][j][1]  = (GLubyte)c;
            myimage[i][j][2]  = (GLubyte)c;
            myimage[i][j][3]  = (GLubyte) 255;
        }
    }

    // load checkerboard image as texture 0
    glBindTexture( GL_TEXTURE_2D, TexID[0] );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 TEX_SIZE, TEX_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, myimage);

    glGenerateMipmap(GL_TEXTURE_2D); // generates mipmaps for the texture attached to target of the active texture unit

    // -------------------------------------------Create 1st normal map--------------------------------------------------------------------
    string extraImage1 = imagePath+"rockNormal.tga";
    int xx1,yy1,nn1;
    unsigned char* extraData = stbi_load((extraImage1).c_str(),&xx1,&yy1,&nn1,0);
    glBindTexture( GL_TEXTURE_2D, TexID[1] );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xx1, yy1, 0, GL_RGB, GL_UNSIGNED_BYTE, extraData);
    stbi_image_free(extraData);
    glGenerateMipmap(GL_TEXTURE_2D);

    // -------------------------------------------Create 2nd normal map--------------------------------------------------------------------
    string extraImage2 = imagePath+"normalMap.tga";
    extraData = stbi_load((extraImage2).c_str(),&xx1,&yy1,&nn1,0);
    glBindTexture( GL_TEXTURE_2D, TexID[2] );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xx1, yy1, 0, GL_RGB, GL_UNSIGNED_BYTE, extraData);
    stbi_image_free(extraData);
    glGenerateMipmap(GL_TEXTURE_2D);

    // -------------------------------------------Create oven & stove texture map----------------------------------------------------------
    string extraImage3 = imagePath+"oven.jpg";
    extraData = stbi_load((extraImage3).c_str(),&xx1,&yy1,&nn1,0);
    glBindTexture( GL_TEXTURE_2D, TexID[3] );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xx1, yy1, 0, GL_RGB, GL_UNSIGNED_BYTE, extraData);
    stbi_image_free(extraData);
    glGenerateMipmap(GL_TEXTURE_2D);

    string extraImage4 = imagePath+"metalSurface.jpg";
    extraData = stbi_load((extraImage4).c_str(),&xx1,&yy1,&nn1,0);
    glBindTexture( GL_TEXTURE_2D, TexID[4] );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xx1, yy1, 0, GL_RGB, GL_UNSIGNED_BYTE, extraData);
    stbi_image_free(extraData);
    glGenerateMipmap(GL_TEXTURE_2D);

    string extraImage5 = imagePath+"stove.jpg";
    extraData = stbi_load((extraImage5).c_str(),&xx1,&yy1,&nn1,0);
    glBindTexture( GL_TEXTURE_2D, TexID[5] );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xx1, yy1, 0, GL_RGB, GL_UNSIGNED_BYTE, extraData);
    stbi_image_free(extraData);
    glGenerateMipmap(GL_TEXTURE_2D);

    // -------------------------------------------Create other textures--------------------------------------------------------------------
    //add more texture map
    int tempCounter = 6;
    for(int i = 0; i < allMTL.size(); i++)
    {
        for(int j = 0; j < allMTL[i].size();j++)
        {
            // load an image from file as texture 1
            string tempImagePath = imagePath+(allMTL[i][j].diffuse_texname);
            int x, y, n;
             unsigned char *data = stbi_load(
                        (tempImagePath).c_str(), /*char* filepath */
                                            &x, /*The address to store the width of the image*/
                                            &y, /*The address to store the height of the image*/
                                            &n  /*Number of channels in the image*/,
                                            0   /*Force number of channels if > 0*/
            );
            glBindTexture( GL_TEXTURE_2D, TexID[tempCounter] );
            if (n == 3) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            }
            else if(n == 4){
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            }
            else {
               cout<<"Image pixels #n = "<<n<<endl;
               fprintf(stderr, "Image pixels are not RGB or Image not exsit \n");
            }
            tempCounter++;
            stbi_image_free(data);

            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }

    // -------------------------------------------Create 2nd texture buffer-------------------------------------------------------------------
    glActiveTexture(GL_TEXTURE1);
    string tempImagePath2 = imagePath+"oven.jpg";
    int xx2, yy2, nn2;
    unsigned char *data2 = stbi_load((tempImagePath2).c_str(),&xx2,&yy2,&nn2,0);
    unsigned int NumOfTex2 = 10;
    TexID2 = new GLuint[NumOfTex2];
    glGenTextures( NumOfTex2, TexID2 );
    glBindTexture(GL_TEXTURE_2D, TexID2[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xx2, yy2, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
    stbi_image_free(data2);
    glGenerateMipmap(GL_TEXTURE_2D);

    string tempImagePath3 = imagePath+"rock.jpg";
    int xx3, yy3, nn3;
    unsigned char *data3 = stbi_load((tempImagePath3).c_str(),&xx3,&yy3,&nn3,0);
    glBindTexture(GL_TEXTURE_2D, TexID2[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xx3, yy3, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
    stbi_image_free(data3);
    glGenerateMipmap(GL_TEXTURE_2D);

    string tempImagePath4 = imagePath+"metalSurface.jpg";
    int xx4, yy4, nn4;
    unsigned char *data4 = stbi_load((tempImagePath4).c_str(),&xx4,&yy4,&nn4,0);
    glBindTexture(GL_TEXTURE_2D, TexID2[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xx4, yy4, 0, GL_RGB, GL_UNSIGNED_BYTE, data4);
    stbi_image_free(data4);
    glGenerateMipmap(GL_TEXTURE_2D);

    string tempImagePath5 = imagePath+"night-sky.jpg";
    int xx5, yy5, nn5;
    unsigned char *data5 = stbi_load((tempImagePath5).c_str(),&xx5,&yy5,&nn5,0);
    glBindTexture(GL_TEXTURE_2D, TexID2[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xx5, yy5, 0, GL_RGB, GL_UNSIGNED_BYTE, data5);
    stbi_image_free(data5);
    glGenerateMipmap(GL_TEXTURE_2D);

    string tempImagePath6 = imagePath+"night-sky.jpg";
    int xx6, yy6, nn6;
    unsigned char *data6 = stbi_load((tempImagePath6).c_str(),&xx6,&yy6,&nn6,0);
    glBindTexture(GL_TEXTURE_2D, TexID2[4]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xx6, yy6, 0, GL_RGB, GL_UNSIGNED_BYTE, data6);
    stbi_image_free(data6);
    glGenerateMipmap(GL_TEXTURE_2D);

    string tempImagePath7 = imagePath+"nightsky_front.jpg";
    int xx7, yy7, nn7;
    unsigned char *data7 = stbi_load((tempImagePath7).c_str(),&xx7,&yy7,&nn7,0);
    glBindTexture(GL_TEXTURE_2D, TexID2[5]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xx7, yy7, 0, GL_RGB, GL_UNSIGNED_BYTE, data7);
    stbi_image_free(data7);
    glGenerateMipmap(GL_TEXTURE_2D);

    string tempImagePath8 = imagePath+"nightsky_back.jpg";
    int xx8, yy8, nn8;
    unsigned char *data8 = stbi_load((tempImagePath8).c_str(),&xx8,&yy8,&nn8,0);
    glBindTexture(GL_TEXTURE_2D, TexID2[6]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xx8, yy8, 0, GL_RGB, GL_UNSIGNED_BYTE, data8);
    stbi_image_free(data8);
    glGenerateMipmap(GL_TEXTURE_2D);

    string tempImagePath9 = imagePath+"nightsky_left.jpg";
    int xx9, yy9, nn9;
    unsigned char *data9 = stbi_load((tempImagePath9).c_str(),&xx9,&yy9,&nn9,0);
    glBindTexture(GL_TEXTURE_2D, TexID2[7]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xx9, yy9, 0, GL_RGB, GL_UNSIGNED_BYTE, data9);
    stbi_image_free(data9);
    glGenerateMipmap(GL_TEXTURE_2D);

    string tempImagePath10 = imagePath+"nightsky_right.jpg";
    int xx10, yy10, nn10;
    unsigned char *data10 = stbi_load((tempImagePath10).c_str(),&xx10,&yy10,&nn10,0);
    glBindTexture(GL_TEXTURE_2D, TexID2[8]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xx10, yy10, 0, GL_RGB, GL_UNSIGNED_BYTE, data10);
    stbi_image_free(data10);
    glGenerateMipmap(GL_TEXTURE_2D);

    string tempImagePath11 = imagePath+"nightsky_top.jpg";
    int xx11, yy11, nn11;
    unsigned char *data11 = stbi_load((tempImagePath11).c_str(),&xx11,&yy11,&nn11,0);
    glBindTexture(GL_TEXTURE_2D, TexID2[9]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xx11, yy11, 0, GL_RGB, GL_UNSIGNED_BYTE, data11);
    stbi_image_free(data11);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Linear interpolation filter gives a smoothed out appearance the square pixels
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // Clamp to edge can help to remove visible seams when lookups are at the edge of the texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // -------------------------------------------Create 3rd texture buffer------------------------------------------------------------
    glActiveTexture(GL_TEXTURE2);
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    // right, left, top, bottom, front, back
    string skyBoxMap1 = imagePath+"right.bmp";
    int sky_x1, sky_y1, sky_n1;
    unsigned char *skyData1 = stbi_load((skyBoxMap1).c_str(),&sky_x1,&sky_y1,&sky_n1,0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, sky_x1, sky_y1, 0, GL_RGB, GL_UNSIGNED_BYTE, skyData1);
    stbi_image_free(skyData1);

    string skyBoxMap2 = imagePath+"left.bmp";
    int sky_x2, sky_y2, sky_n2;
    unsigned char *skyData2 = stbi_load((skyBoxMap2).c_str(),&sky_x2,&sky_y2,&sky_n2,0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, sky_x2, sky_y2, 0, GL_RGB, GL_UNSIGNED_BYTE, skyData2);
    stbi_image_free(skyData2);

    string skyBoxMap3 = imagePath+"top.bmp";
    int sky_x3, sky_y3, sky_n3;
    unsigned char *skyData3 = stbi_load((skyBoxMap3).c_str(),&sky_x3,&sky_y3,&sky_n3,0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, sky_x3, sky_y3, 0, GL_RGB, GL_UNSIGNED_BYTE, skyData3);
    stbi_image_free(skyData3);

    string skyBoxMap4 = imagePath+"bottom.bmp";
    int sky_x4, sky_y4, sky_n4;
    unsigned char *skyData4 = stbi_load((skyBoxMap4).c_str(),&sky_x4,&sky_y4,&sky_n4,0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, sky_x4, sky_y4, 0, GL_RGB, GL_UNSIGNED_BYTE, skyData4);
    stbi_image_free(skyData4);

    string skyBoxMap5 = imagePath+"front.bmp";
    int sky_x5, sky_y5, sky_n5;
    unsigned char *skyData5 = stbi_load((skyBoxMap5).c_str(),&sky_x5,&sky_y5,&sky_n5,0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, sky_x5, sky_y5, 0, GL_RGB, GL_UNSIGNED_BYTE, skyData5);
    stbi_image_free(skyData5);

    string skyBoxMap6 = imagePath+"back.bmp";
    int sky_x6, sky_y6, sky_n6;
    unsigned char *skyData6 = stbi_load((skyBoxMap6).c_str(),&sky_x6,&sky_y6,&sky_n6,0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, sky_x6, sky_y6, 0, GL_RGB, GL_UNSIGNED_BYTE, skyData6);
    stbi_image_free(skyData6);

    // setup texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


/**
 * @brief updateProjection
 * @return int
 * This function is used to update projection view.
 */
int updateProjection()
{
    // Perspective projection matrix
    projection = glm::perspective((double)FOV, double(winW) / double(winH), 0.01, 100.0);
    
	// Load it to the shader program
	int projHandle = glGetUniformLocation(ProgramID, "projection_matrix");
	if (projHandle == -1) {
        fprintf(stderr, "Error updating proj matrix\n");
        return 1;
    }
    glUniformMatrix4fv( projHandle, 1, false, glm::value_ptr(projection) );

    return 0;
}

/**
 * @brief ReshapeFunc
 * @param x
 * @param y
 * This function is used to reshape window.
 */
void ReshapeFunc( int x, int y )
{
    // ensure y never gose zero
    if(y == 0)
    {
        y = 1;
    }
    winW = x;
    winH = y;
    glViewport(0, 0, (GLsizei)x, (GLsizei)y); // Set our viewport to the size of our window
    updateProjection();
}

/**
 * @brief TimerFunc
 * @param value
 * Time function.
 */
void TimerFunc(int value)
{
    glutPostRedisplay();
    glutTimerFunc( 33, TimerFunc, value );
}

/**
 * @brief MouseFunc
 * @param button
 * @param state
 * @param x
 * @param y
 * Mouse function.
 */
void MouseFunc(int button, int state, int x, int y){
    // Update a global set of variables
    // to avoid cluttering up the callbacks
    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN)
            {
                Input.lMousePressed = true;
                Input.prevX = x;
                Input.prevY = y;
            }
            else{
                Input.lMousePressed = false;
            }
            break;
        case GLUT_RIGHT_BUTTON:
            if (state == GLUT_DOWN)
            {
                Input.rMousePressed = true;
            }
            else{
                Input.rMousePressed = false;
            }
            break;
        default:
            break;
    }
}

/**
 * @brief MotionFunc
 * @param x
 * @param y
 * This function is used to transfer mouse moving data
 * and limit the max/min zoom range.
 */
void MotionFunc(int x, int y)
{
    int xDiff = x - Input.prevX;
    int yDiff = y - Input.prevY;
    Input.deltaX += xDiff;
    Input.deltaY += yDiff;
    Input.prevX = x;
    Input.prevY = y;
}

/**
 * @brief TogglePolygon
 * This function is used to control polygon mode.
 */
void TogglePolygon()
{
    if(checkMode == true)
    {
        // WireFrame on
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else if(checkMode == false)
    {
        // WireFrame off
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

/**
 * @brief scaleModel
 * @param tempVec
 * @return float
 * This function is used to find model size scale.
 */
float scaleModel(vector<modelDataVAO*> tempVec)
{
    // Calculate max and min value of model
    float tempX_max = 0;
    float tempX_min = 0;
    float tempY_max = 0;
    float tempY_min = 0;
    float tempZ_max = 0;
    float tempZ_min = 0;
    for(int i = 0; i < tempVec.size(); i++)
    {
        if(tempVec[i]->modelSize()[0] > tempX_max){
            tempX_max = tempVec[i]->modelSize()[0];
        }

        if(tempVec[i]->modelSize()[1] < tempX_min){
            tempX_min = tempVec[i]->modelSize()[1];
        }

        if(tempVec[i]->modelSize()[2] > tempY_max){
            tempY_max = tempVec[i]->modelSize()[2];
        }

        if(tempVec[i]->modelSize()[3] < tempY_min){
            tempY_min = tempVec[i]->modelSize()[3];
        }

        if(tempVec[i]->modelSize()[4] > tempZ_max){
            tempZ_max = tempVec[i]->modelSize()[4];
        }

        if(tempVec[i]->modelSize()[5] < tempZ_min){
            tempZ_min = tempVec[i]->modelSize()[5];
        }
    }

    std::vector<float> tempS;
    tempS.push_back(tempX_max);
    tempS.push_back(tempX_min);
    tempS.push_back(tempY_max);
    tempS.push_back(tempY_min);
    tempS.push_back(tempZ_max);
    tempS.push_back(tempZ_min);

    for(int i = 0; i < 6; i++)
    {
        if(tempS[i] < 0)
        {
            tempS[i] = 0 - tempS[i];
        }    
    }

    float tmpRes = 1;
    for(int i = 0; i < 6; i++)
    {
        if(tempS[i] <0.5 && tempS[i] != 0)
        {
            tmpRes = tempS[i];
        }
        else if(tempS[i] >= 3.5)
        {
            tmpRes = tempS[i];
        }
    }
    return tmpRes;
}



/**
 * Sets the shader uniforms and generic attributes
 * @param program, Shader program object to use
 */
int setup()
{
	// Perspective projection matrix
    updateProjection();
    wave = 0.0f;
    // create sphere
    sphere = new Sphere(1.0f, 32, 32);
    createSphereVAO(ProgramID,checkMode);

    // Create the VAO representing square with tex coords
    // ---------------------------------------------Object 1 - 3----------------------------------------------------
    for(int i = 0; i < shapes.size(); i++)
    {
        if(shapes[i].mesh.material_ids[0] < 0)
        {
            shapeData = new modelDataVAO(shapes[i],materials[0], ProgramID);
            modelVector.push_back(shapeData);
        }
        else{
            shapeData = new modelDataVAO(shapes[i],materials[shapes[i].mesh.material_ids[0]], ProgramID);
            modelVector.push_back(shapeData);
        }
    }
    for(int i = 0; i < shapes2.size(); i++)
    {
        if(shapes2[i].mesh.material_ids[0] < 0){
            shapeData = new modelDataVAO(shapes2[i], materials2[0],ProgramID);
            modelVector2.push_back(shapeData);
        }
        else{
            shapeData = new modelDataVAO(shapes2[i], materials2[shapes2[i].mesh.material_ids[0]],ProgramID);
            modelVector2.push_back(shapeData);
        }
    }
    for(int i = 0; i < shapes3.size(); i++)
    {
        if(shapes3[i].mesh.material_ids[0] < 0)
        {
            shapeData = new modelDataVAO(shapes3[i], materials3[0], ProgramID);
            modelVector3.push_back(shapeData);
        }
        else{
            shapeData = new modelDataVAO(shapes3[i], materials3[shapes3[i].mesh.material_ids[0]], ProgramID);
            modelVector3.push_back(shapeData);
        }
    }
    // ---------------------------------------------Robot-----------------------------------------------------------
    for(int i = 0; i < shapes4.size(); i++)
    {
        if(shapes4[i].mesh.material_ids[0] < 0)
        {
            shapeData = new modelDataVAO(shapes4[i], materials4[0], ProgramID);
            modelVector4.push_back(shapeData);
        }
        else{
            shapeData = new modelDataVAO(shapes4[i], materials4[shapes4[i].mesh.material_ids[0]], ProgramID);
            modelVector4.push_back(shapeData);
        }
    }
    // ---------------------------------------------Robot MIRROR ---------------------------------------------------
    for(int i = 0; i < shapes4.size(); i++)
    {
        if(shapes4[i].mesh.material_ids[0] < 0)
        {
            shapeData = new modelDataVAO(shapes4[i], materials4[0], ProgramID6);
            modelVector4MIRROR.push_back(shapeData);
        }
        else{
            shapeData = new modelDataVAO(shapes4[i], materials4[shapes4[i].mesh.material_ids[0]], ProgramID6);
            modelVector4MIRROR.push_back(shapeData);
        }
    }
    // ---------------------------------------------object 5--------------------------------------------------------
    for(int i = 0; i < shapes5.size(); i++)
    {
        if(shapes5[i].mesh.material_ids[0] < 0)
        {
            shapeData = new modelDataVAO(shapes5[i], materials5[0], ProgramID);
            modelVector5.push_back(shapeData);
        }
        else{
            shapeData = new modelDataVAO(shapes5[i], materials5[shapes5[i].mesh.material_ids[0]], ProgramID);
            modelVector5.push_back(shapeData);
        }
    }
    // --------------------------------------------object 6---------------------------------------------------------
    for(int i = 0; i < shapes6.size(); i++)
    {
        if(shapes6[i].mesh.material_ids[0] < 0)
        {
            shapeData = new modelDataVAO(shapes6[i], materials6[0], ProgramID);
            modelVector6.push_back(shapeData);
        }
        else{
            shapeData = new modelDataVAO(shapes6[i], materials6[shapes6[i].mesh.material_ids[0]], ProgramID);
            modelVector6.push_back(shapeData);
        }
    }

    // ---------------------------------------------object 7--------------------------------------------------------
   for(int i = 0; i < shapes7.size(); i++)
   {
       if(shapes7[i].mesh.material_ids[0] < 0)
       {
           shapeData = new modelDataVAO(shapes7[i], materials7[0], ProgramID);
           modelVector7.push_back(shapeData);
       }
       else{
           shapeData = new modelDataVAO(shapes7[i], materials7[shapes7[i].mesh.material_ids[0]], ProgramID);
           modelVector7.push_back(shapeData);
       }
   }
    // ---------------------------------------------Scale the mode--------------------------------------------------
    float finalScale = scaleModel(modelVector)*55;
    float finalScale2 = scaleModel(modelVector2)*40;
    float finalScale3 = scaleModel(modelVector3)*28;
    float finalScale4 = scaleModel(modelVector4)*15;
    float finalScale5 = scaleModel(modelVector5)*130;
    float finalScale6 = scaleModel(modelVector6)*50;
    float finalScale7 = scaleModel(modelVector7);

    scales.push_back(finalScale);
    scales.push_back(finalScale2);
    scales.push_back(finalScale3);
    scales.push_back(finalScale4);
    scales.push_back(finalScale5);
    scales.push_back(finalScale6);
    scales.push_back(finalScale7);

    // ---------------------------------------------Create skybox texture-------------------------------------------
    TexID3 = createTextures();

    createAxis();
    // ---------------------------------------------Setup other objects VAO-----------------------------------------
    plain = new Terrain();
    plain->createTerrianVAO(ProgramID3,checkMode);
    plain2 = new Terrain();
    plain2->createTerrianVAO(ProgramID4,checkMode);
    glassPlain = new Terrain();
    glassPlain->createTerrianVAO(ProgramID,checkMode);

    ovenPlain = new Terrain();
    ovenPlain->createTerrianVAO(ProgramID,checkMode);

    stovePlain = new Terrain();
    stovePlain->createTerrianVAO(ProgramID,checkMode);

    for(int i = 0; i < shapes.size();i++)
    {
        modelVector[i]->createVAO(ProgramID, checkMode);
    }

    for(int i = 0; i < shapes2.size();i++)
    {
        modelVector2[i]->createVAO(ProgramID, checkMode);
    }

    for(int i = 0; i < shapes3.size(); i++)
    {
        modelVector3[i]->createVAO(ProgramID, checkMode);
    }

    // ---------------------------------------Robot & MIRROR----------------------------------------------------------------
    for(int i = 0; i < shapes4.size(); i++)
    {
        modelVector4[i]->createVAO(ProgramID, checkMode, finalScale4);
    }

    for(int i = 0; i < shapes4.size(); i++)
    {
        modelVector4MIRROR[i]->createVAO(ProgramID6, checkMode, finalScale4);
    }

    // ---------------------------------------------------------------------------------------------------------------------
    for(int i = 0; i < shapes5.size(); i++)
    {
        modelVector5[i]->createVAO(ProgramID, checkMode);
    }

    for(int i = 0; i < shapes6.size(); i++)
    {
        modelVector6[i]->createVAO(ProgramID, checkMode);
    }

    for(int i = 0; i < shapes7.size(); i++)
    {
        modelVector7[i]->createVAO(ProgramID, checkMode);
    }

    // ---------------------------------------Setup light properties--------------------------------------------------------
    std::vector<float> lightambient;
    std::vector<float> lightdiffuse;
    std::vector<float> lightspecular;

    //white light
    float templightambient1 = 0.0f;
    float templightambient2 = 0.0f;
    float templightambient3 = 0.0f;
    lightambient.push_back(templightambient1);
    lightambient.push_back(templightambient2);
    lightambient.push_back(templightambient3);

    float lightdiffuse1 = 1.0f;
    float lightdiffuse2 = 1.0f;
    float lightdiffuse3 = 1.0f;
    lightdiffuse.push_back(lightdiffuse1);
    lightdiffuse.push_back(lightdiffuse2);
    lightdiffuse.push_back(lightdiffuse3);

    float lightspecular1 = 1.0f;
    float lightspecular2 = 1.0f;
    float lightspecular3 = 1.0f;
    lightspecular.push_back(lightspecular1);
    lightspecular.push_back(lightspecular2);
    lightspecular.push_back(lightspecular3);

    bulb = new lightSource(ProgramID,lightambient,lightdiffuse,lightspecular, 24);
    bulb2 = new lightSource(ProgramID3,lightambient,lightdiffuse,lightspecular, 4);
    bulb3 = new lightSource(ProgramID4, lightambient,lightdiffuse,lightspecular, 1);

    // lamp 1 light 1-6
    glm::vec3 pos = glm::vec3(0.0f,0.143f,0.0f);
    glm::vec3 pos2 = glm::vec3(0.007f, 0.15f, 0.0f);
    glm::vec3 pos3 = glm::vec3(-0.007f, 0.149f, 0.0f);
    glm::vec3 pos4 = glm::vec3(0.0f, 0.149f, 0.007f);
    glm::vec3 pos5 = glm::vec3(0.0f, 0.149, -0.007);
    glm::vec3 pos6 = glm::vec3(0.0f, 0.158f, 0.0f);

    // lamp 2 light 1-6
    glm::vec3 pos7 = glm::vec3(0.18f,0.143f,0.0f);
    glm::vec3 pos8 = glm::vec3(0.18f,0.149f,0.007f);
    glm::vec3 pos9 = glm::vec3(0.18f,0.149f,-0.007f);
    glm::vec3 pos10 = glm::vec3(0.173f,0.149f,0.0f);
    glm::vec3 pos11 = glm::vec3(0.187f,0.149f,0.0f);
    glm::vec3 pos12 = glm::vec3(0.18f, 0.158f, 0.0f);

    // lamp 3 light 1-6
    glm::vec3 pos13 = glm::vec3(-0.719994f,0.142f,0.0f);
    glm::vec3 pos14 = glm::vec3(-0.719994f,0.149f,0.007f);
    glm::vec3 pos15 = glm::vec3(-0.719994f,0.149f,-0.007f);
    glm::vec3 pos16 = glm::vec3(-0.726994f,0.149f,0.0f);
    glm::vec3 pos17 = glm::vec3(-0.712994f,0.149f,0.0f);
    glm::vec3 pos18 = glm::vec3(-0.719994f, 0.158f, 0.0f);

    // lamp 4 light 1-6
    glm::vec3 pos19 = glm::vec3(-0.899992f,0.142f,0.0f);
    glm::vec3 pos20 = glm::vec3(-0.899992f,0.149f,0.007f);
    glm::vec3 pos21 = glm::vec3(-0.899992f,0.149f,-0.007f);
    glm::vec3 pos22 = glm::vec3(-0.906992f,0.149f,0.0f);
    glm::vec3 pos23 = glm::vec3(-0.892992f,0.149f,0.0f);
    glm::vec3 pos24 = glm::vec3(-0.899992f, 0.158f, 0.0f);

    std::vector<glm::vec3> LPos;
    LPos.push_back(pos);
    LPos.push_back(pos2);
    LPos.push_back(pos3);
    LPos.push_back(pos4);
    LPos.push_back(pos5);
    LPos.push_back(pos6);
    LPos.push_back(pos7);
    LPos.push_back(pos8);
    LPos.push_back(pos9);
    LPos.push_back(pos10);
    LPos.push_back(pos11);
    LPos.push_back(pos12);
    LPos.push_back(pos13);
    LPos.push_back(pos14);
    LPos.push_back(pos15);
    LPos.push_back(pos16);
    LPos.push_back(pos17);
    LPos.push_back(pos18);
    LPos.push_back(pos19);
    LPos.push_back(pos20);
    LPos.push_back(pos21);
    LPos.push_back(pos22);
    LPos.push_back(pos23);
    LPos.push_back(pos24);

    bulb->setLightPos(LPos);

    lightLeft = 0.0f;
    lightup = 0.0f;
    lightForward = 0.0f;
    createCube();
    return 0;	// return success
}

/**
 * @brief draw
 * main drawing function
 */
void draw()
{
    viewMatrix = glm::mat4(Camera->getViewMtx());
    // -------------------------------Wireframe shader Axis------------------------------------------------------
    if(checkMode == false)
    {
        // shader 2
        // Draw world axis and switch shader
        glUseProgram(ProgramID2);

        // Load it to the shader program
        int projHandle2 = glGetUniformLocation(ProgramID2, "projection_matrix");
        glUniformMatrix4fv( projHandle2, 1, false, glm::value_ptr(projection) );

        // View matrix
        int viewHandle2 = glGetUniformLocation(ProgramID2, "view_matrix");
        glUniformMatrix4fv(viewHandle2, 1, false, glm::value_ptr(viewMatrix));

        // start draw axis
        glBindVertexArray(axisVaoHandle);

        // scaling matrix
        int modelHandle2 = glGetUniformLocation(ProgramID2, "model_matrix");
        glm::mat4 axisModel = glm::mat4(1.0);
        axisModel = glm::scale(axisModel, glm::vec3(1,1,1));
        glUniformMatrix4fv(modelHandle2, 1, false, glm::value_ptr(axisModel));
        glDrawArrays(GL_LINES, 0, 18);
    }

    // ------------------------------- Skybox Cueb mapping------------------------------------------------------
    glDisable(GL_DEPTH_TEST);
    glUseProgram(ProgramID5);

    // Load it to the shader program
    int projHandle5 = glGetUniformLocation(ProgramID5, "projection_matrix");
    glUniformMatrix4fv( projHandle5, 1, false, glm::value_ptr(projection) );

    // View matrix
    int viewHandle5 = glGetUniformLocation(ProgramID5, "view_matrix");
    glUniformMatrix4fv( viewHandle5, 1, false, glm::value_ptr(viewMatrix) );

    glBindVertexArray(skyBoxVaoHandle);

    int skyBoxUniformHandle = glGetUniformLocation(ProgramID5,"cubeMap");

    if(skyBoxUniformHandle == -1)
    {
        cout<<"Can't find skybox uniform matrix"<<endl;
        cout<<"skyBoxUniformHandle = "<<skyBoxUniformHandle<<endl;
        exit(1);
    }

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP,TexID3);
    glUniform1i(skyBoxUniformHandle, 2);

    int CubeModelHandle = glGetUniformLocation(ProgramID5, "model_matrix");
    glm::mat4 CubeModel = glm::mat4(1.0);
    CubeModel = glm::scale(CubeModel, glm::vec3(5,5,5));
    glUniformMatrix4fv(CubeModelHandle, 1, false, glm::value_ptr(CubeModel));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    // -----------------------------Textured shader--------------------------------------------------------------
    // switch shader and draw objects and textures
    unsigned int currentProgram = ProgramID;
    if(checkMode == false)
    {
        currentProgram = ProgramID2;
    }
    else if(checkMode == true)
    {
        currentProgram = ProgramID;
    }
    glUseProgram(currentProgram);

    // pass the camera view
    int viewUniformHandle = glGetUniformLocation(currentProgram,"view_matrix");
    glUniformMatrix4fv(viewUniformHandle, 1, false, glm::value_ptr(viewMatrix));

    int texHandle = glGetUniformLocation(ProgramID, "texMap");
    //We are using texture unit 0 (the default)
    glActiveTexture(GL_TEXTURE0);

    // -------------------------------Draw 1st object---------------------------------------------------------------
    for(int i = 0; i < shapes.size(); i++)
    {
        //only works for shader 1
        if(checkMode == true)
        {
            if(modelVector[i]->getMatID() < 0 || modelVector[i]->material.diffuse_texname == "")
            {
                //use default white
                glBindTexture(GL_TEXTURE_2D, TexID[0]);
            }
            else{
                glBindTexture(GL_TEXTURE_2D, TexID[modelVector[i]->getMatID()+6]);
            }
        }

        glBindVertexArray(modelVector[i]->getVaoHandle());

        // setup position of model
        std::vector<float> tmpAngle;
        tmpAngle.push_back(-1.57f);
        std::vector<glm::vec3> tmpRotAxis;
        tmpRotAxis.push_back(glm::vec3(0.0f,1.0f,0.0f));
        modelVector[i]->render(currentProgram,scales[0],viewMatrix, checkMode, lightingEffectSwitch[0],tempScaleVec, glm::vec3(3.0f,0,-4.0f), tmpAngle, tmpRotAxis);
    }
    // -------------------------------Draw 2nd object---------------------------------------------------------------
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    for(int i = 0; i < shapes2.size(); i++)
    {
        //only works for shader 1
        if(checkMode == true)
        {
            if(modelVector2[i]->getMatID() < 0 || modelVector2[i]->material.diffuse_texname == "")
            {
                //use default white
                glBindTexture(GL_TEXTURE_2D, TexID[0]);
            }
            else{
                glBindTexture(GL_TEXTURE_2D, TexID[modelVector2[i]->getMatID()+allMTL[0].size()+6]);
            }
        }

        glBindVertexArray(modelVector2[i]->getVaoHandle());

        // setup position
        std::vector<float> tmpAngle;
        tmpAngle.push_back(0.0f);
        std::vector<glm::vec3> tmpRotAxis;
        tmpRotAxis.push_back(glm::vec3(1.0f,0.0f,0.0f));
        modelVector2[i]->render(currentProgram,scales[1],viewMatrix, checkMode, 5.0f,tempScaleVec, glm::vec3(-14.0f,3.5f,-6.5f), tmpAngle, tmpRotAxis);
    }

    glDisable(GL_BLEND);

    // -------------------------------Draw 3rd object---------------------------------------------------------------
    for(int i = 0; i < shapes3.size(); i++)
    {
        //only works for shader 1
        if(checkMode == true)
        {
            if(modelVector3[i]->getMatID() < 0 || modelVector3[i]->material.diffuse_texname == "")
            {
                //use default white
                glBindTexture(GL_TEXTURE_2D, TexID[0]);
            }
            else{
                glBindTexture(GL_TEXTURE_2D, TexID[modelVector3[i]->getMatID()+allMTL[0].size()+allMTL[1].size()+6]);
            }
        }

        glBindVertexArray(modelVector3[i]->getVaoHandle());

        // setup position
        std::vector<float> tmpAngle;
        tmpAngle.push_back(1.57f);
        std::vector<glm::vec3> tmpRotAxis;
        tmpRotAxis.push_back(glm::vec3(0.0f,1.0f,0.0f));
        modelVector3[i]->render(currentProgram,scales[2],viewMatrix, checkMode, lightingEffectSwitch[0],tempScaleVec, glm::vec3(-3.0f,0.0f,0.0f), tmpAngle, tmpRotAxis);
    }

    // -------------------------------Draw 4th object AND update Robot view------------------------------------------
    // setup viewMtx for moving robot
    // setup user view to be the FPS view mode
    if(camMode == 1)
    {
        robotPos = Camera->getCamPos();
        robotPos.y = robotPos.y - 0.05f;
        //robotPos.z = robotPos.z + 0.05f;
    }
    else{
        robotPos = Camera->getRobotPos();
    }

    // Start draw 4th object
    for(int i = 0; i < shapes4.size(); i++)
    {
        //only works for shader 1
        if(checkMode == true)
        {
            if(modelVector4[i]->getMatID() < 0 || modelVector4[i]->material.diffuse_texname == "")
            {
                //use default white
                glBindTexture(GL_TEXTURE_2D, TexID[0]);
            }
            else{
                glBindTexture(GL_TEXTURE_2D, TexID[modelVector4[i]->getMatID()+allMTL[0].size()+allMTL[1].size()+allMTL[2].size()+6]);
            }
        }

        glBindVertexArray(modelVector4[i]->getVaoHandle());

        // setup robot
        modelVector4[i]->render(currentProgram,viewMatrix, checkMode, lightingEffectSwitch[0], robotPos, 0.0f, glm::vec3(1,0,0));
    }

    // -------------------------------Draw 5th object ---------------------------------------------------------------
    for(int i = 0; i < shapes5.size(); i++)
    {
        //only works for shader 1
        if(checkMode == true)
        {
            if(modelVector5[i]->getMatID() < 0 || modelVector5[i]->material.diffuse_texname == "")
            {
                //use default white
                glBindTexture(GL_TEXTURE_2D, TexID[0]);
            }
            else{
                glBindTexture(GL_TEXTURE_2D, TexID[modelVector5[i]->getMatID()+allMTL[0].size()+allMTL[1].size()+allMTL[2].size()+allMTL[3].size()+6]);             
            }
        }

        glBindVertexArray(modelVector5[i]->getVaoHandle());

        // setup position
        std::vector<float> tmpAngle;
        tmpAngle.push_back(-1.57f);
        tmpAngle.push_back(-1.57f);
        std::vector<glm::vec3> tmpRotAxis;
        tmpRotAxis.push_back(glm::vec3(1.0f,0.0f,0.0f));
        tmpRotAxis.push_back(glm::vec3(0.0f,0.0f,1.0f));
        modelVector5[i]->render(currentProgram,scales[4],viewMatrix, checkMode, lightingEffectSwitch[0],tempScaleVec, glm::vec3(0.0f,0.0f,0.0f), tmpAngle, tmpRotAxis);
    }
    // -------------------------------Draw 6th object ---------------------------------------------------------------
    for(int i = 0; i < shapes6.size(); i++)
    {
        //only works for shader 1
        if(checkMode == true)
        {
            if(modelVector6[i]->getMatID() < 0 || modelVector6[i]->material.diffuse_texname == "")
            {
                //use default white
                glBindTexture(GL_TEXTURE_2D, TexID[0]);
            }
            else{
                glBindTexture(GL_TEXTURE_2D, TexID[modelVector6[i]->getMatID()+allMTL[0].size()+allMTL[1].size()+allMTL[2].size()+allMTL[3].size()+allMTL[4].size()+6]);
            }
        }

        glBindVertexArray(modelVector6[i]->getVaoHandle());

        // setup position
        std::vector<float> tmpAngle;
        tmpAngle.push_back(0.0f);
        std::vector<glm::vec3> tmpRotAxis;
        tmpRotAxis.push_back(glm::vec3(1.0f,0.0f,0.0f));
        modelVector6[i]->render(currentProgram,scales[5],viewMatrix, checkMode, lightingEffectSwitch[0],tempScaleVec, glm::vec3(0.0f,25.0f,0.0f), tmpAngle, tmpRotAxis);

        modelVector6[i]->render(currentProgram,scales[5],viewMatrix, checkMode, lightingEffectSwitch[0],tempScaleVec, glm::vec3(30.0f,25.0f,0.0f), tmpAngle, tmpRotAxis);

        modelVector6[i]->render(currentProgram,scales[5],viewMatrix, checkMode, lightingEffectSwitch[0],tempScaleVec, glm::vec3(-120.0f,25.0f,0.0f), tmpAngle, tmpRotAxis);

        modelVector6[i]->render(currentProgram,scales[5],viewMatrix, checkMode, lightingEffectSwitch[0],tempScaleVec, glm::vec3(-150.0f,25.0f,0.0f), tmpAngle, tmpRotAxis);
    }

    // -------------------------------Draw 7th object ---------------------------------------------------------------

    for(int i = 0; i < shapes7.size(); i++)
    {
        //only works for shader 1
        if(checkMode == true)
        {
            if(modelVector7[i]->getMatID() < 0 || modelVector7[i]->material.diffuse_texname == "")
            {
                //use default white
                glBindTexture(GL_TEXTURE_2D, TexID[0]);
            }
            else{
                glBindTexture(GL_TEXTURE_2D, TexID[modelVector7[i]->getMatID()+allMTL[0].size()+allMTL[1].size()+allMTL[2].size()+allMTL[3].size()+allMTL[4].size()+allMTL[5].size()+6]);
            }
        }

        glBindVertexArray(modelVector7[i]->getVaoHandle());

        // setup position
        std::vector<float> tmpAngle;
        tmpAngle.push_back(wave*3);
        std::vector<glm::vec3> tmpRotAxis;
        tmpRotAxis.push_back(glm::vec3(1.0f,0.0f,0.0f));
        modelVector7[i]->render(currentProgram,scales[6],viewMatrix, checkMode, 6.0f,tempScaleVec, glm::vec3(0.0f,-2.0f,0.0f), tmpAngle, tmpRotAxis);
    }


    // -------------------------------Draw oven object --------------------------------------------------------------------
    glBindVertexArray(ovenPlain->getTerrainVAOHandle());
    // right side
    glBindTexture(GL_TEXTURE_2D, TexID[4]);
    ovenPlain->render(currentProgram,0.01,checkMode,lightingEffectSwitch[0],tempScaleVec,glm::vec3(-0.089*100,0.074*100,-0.01*100),1.57f,glm::vec3(1.0f,0.0f,0.0f), 32.0f);
    // top side
    glBindTexture(GL_TEXTURE_2D, TexID[5]);
    ovenPlain->render(currentProgram,0.01,checkMode,4.0f,tempScaleVec,glm::vec3(0.064*100,0.02*100,0.089*100),-1.57f,glm::vec3(0.0f,1.0f,0.0f), 32.0f);
    // back side
    glBindTexture(GL_TEXTURE_2D, TexID[4]);
    ovenPlain->render(currentProgram,0.01,checkMode,lightingEffectSwitch[0],tempScaleVec,glm::vec3(0.01*100,0.099*100,0.064*100),1.57f,glm::vec3(0.0f,0.0f,1.0f), 32.0f);
    // left side
    glBindTexture(GL_TEXTURE_2D, TexID[4]);
    ovenPlain->render(currentProgram,0.01,checkMode,lightingEffectSwitch[0],tempScaleVec,glm::vec3(-0.089*100,0.054*100,-0.01*100),1.57f,glm::vec3(1.0f,0.0f,0.0f), 32.0f);
    // front side
    glBindTexture(GL_TEXTURE_2D, TexID[3]);
    ovenPlain->render(currentProgram,0.01,checkMode,4.0f,tempScaleVec,glm::vec3(0.01*100,0.079*100,0.064*100),1.57f,glm::vec3(0.0f,0.0f,1.0f), 32.0f);

    // setup texture0 unit
    glUniform1i(texHandle, 0);

    // -------------------------------Draw glass windows --------------------------------------------------------------------
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glass wall inside
    glassPlain->render(ProgramID,0.2f,checkMode,2.0f,tempScaleVec,glm::vec3(0.0f,1.99f,-1.01f), 1.57f, glm::vec3(1.0f,0.0f,0.0f),  32.0f);
    glassPlain->render(ProgramID,0.4f,checkMode,2.0f,tempScaleVec,glm::vec3(0.0f,1.001f,-1.0f), 1.57f, glm::vec3(1.0f,0.0f,0.0f),  32.0f);

    // front
    glassPlain->render(ProgramID,0.2f,checkMode,1.0f,tempScaleVec,glm::vec3(1.01f,-1.99f,0.0f), 1.57f, glm::vec3(0.0f,0.0f,1.0f),  32.0f);
    glassPlain->render(ProgramID,0.4f,checkMode,2.0f,tempScaleVec,glm::vec3(-1.0f,1.001f,0.0f), -1.57f, glm::vec3(0.0f,0.0f,1.0f),  32.0f);

    glassPlain->render(ProgramID,0.4f,checkMode,2.0f,tempScaleVec,glm::vec3(0.0f,1.001f,1.0f), -1.57f, glm::vec3(1.0f,0.0f,0.0f),  32.0f);


    // glass wall ouside
    // 2nd room back
    glassPlain->render(ProgramID,0.4f,checkMode,2.0f,tempScaleVec,glm::vec3(1.0f,3.001f,0.0f), 1.57f, glm::vec3(0.0f,0.0f,1.0f),  32.0f);
    // right
    glassPlain->render(ProgramID,0.4f,checkMode,2.0f,tempScaleVec,glm::vec3(-2.0f,1.001f,-1.0f), 1.57f, glm::vec3(1.0f,0.0f,0.0f),  32.0f);
    // left
    glassPlain->render(ProgramID,0.4f,checkMode,2.0f,tempScaleVec,glm::vec3(-2.0f,1.002f,1.0f), -1.57f, glm::vec3(1.0f,0.0f,0.0f),  32.0f);
    // bottom
    glassPlain->render(ProgramID,0.4f,checkMode,2.0f,tempScaleVec,glm::vec3(-2.0f,-0.001f,0.0f), 0.0f, glm::vec3(0.0f,0.0f,1.0f),  32.0f);

    glDisable(GL_BLEND);


    // -------------------------------setup light properties ----------------------------------------------------------------

    bulb->lightRender();

    // -------------------------------Draw wall object ----------------------------------------------------------------------
    glUseProgram(ProgramID3);

    // Perspective projection matrix
    glm::mat4 projection3;
    projection3 = glm::perspective((double)FOV, double(winW) / double(winH), 0.01, 100.0);

    // Load it to the shader program
    int projHandle3 = glGetUniformLocation(ProgramID3, "projection_matrix");
    glUniformMatrix4fv( projHandle3, 1, false, glm::value_ptr(projection) );

    // View matrix
    int viewHandle3 = glGetUniformLocation(ProgramID3, "view_matrix");
    glUniformMatrix4fv( viewHandle3, 1, false, glm::value_ptr(viewMatrix) );

    // start draw wall
    int textureHandle0 = glGetUniformLocation(ProgramID3,"texMap0");
    int textureHandle1 = glGetUniformLocation(ProgramID3,"texMap1");

    if(textureHandle0 == -1 || textureHandle1 == -1)
    {
        cout<<"Can't find wall uniform matrix"<<endl;
        cout<<"texMap0 = "<<textureHandle0<<endl;
        cout<<"texMap1 = "<<textureHandle1<<endl;
        exit(1);
    }

    // example wall
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TexID[1]);
    glUniform1i(textureHandle0, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, TexID2[1]);
    glUniform1i(textureHandle1, 1);

    // right wall
    plain->render(ProgramID3,0.4f,checkMode,lightingEffectSwitch[0],tempScaleVec,glm::vec3(0.0f,1.0f,-1.0f), 1.57f, glm::vec3(1.0f,0.0f,0.0f),  32.0f);
    plain->render(ProgramID3,0.4f,checkMode,lightingEffectSwitch[0],tempScaleVec,glm::vec3(-2.0f,1.0f,-1.0f), 1.57f, glm::vec3(1.0f,0.0f,0.0f),  32.0f);

    // left wall
    plain->render(ProgramID3,0.4f,checkMode,lightingEffectSwitch[0],tempScaleVec,glm::vec3(0.0f,1.0f,1.0f), -1.57f, glm::vec3(1.0f,0.0f,0.0f),  32.0f);
    plain->render(ProgramID3,0.4f,checkMode,lightingEffectSwitch[0],tempScaleVec,glm::vec3(-2.0f,1.0f,1.0f), -1.57f, glm::vec3(1.0f,0.0f,0.0f),  32.0f);

    // front wall
    plain->render(ProgramID3,0.4f,checkMode,lightingEffectSwitch[0],tempScaleVec,glm::vec3(-1.0f,1.0f,0.0f), -1.57f, glm::vec3(0.0f,0.0f,1.0f),  32.0f);

    // mid wall
    plain->render(ProgramID3,0.1f,checkMode,lightingEffectSwitch[0],tempScaleVec,glm::vec3(1.0f,4.0f,-3.0f), 1.57f, glm::vec3(0.0f,0.0f,1.0f),  32.0f);
    plain->render(ProgramID3,0.1f,checkMode,lightingEffectSwitch[0],tempScaleVec,glm::vec3(1.0f,4.0f,3.0f), 1.57f, glm::vec3(0.0f,0.0f,1.0f),  32.0f);

    plain->render(ProgramID3,0.1f,checkMode,lightingEffectSwitch[0],tempScaleVec,glm::vec3(7.0f,4.0f,-3.0f), 1.57f, glm::vec3(0.0f,0.0f,1.0f),  32.0f);
    plain->render(ProgramID3,0.1f,checkMode,lightingEffectSwitch[0],tempScaleVec,glm::vec3(7.0f,4.0f,-1.0f), 1.57f, glm::vec3(0.0f,0.0f,1.0f),  32.0f);
    plain->render(ProgramID3,0.1f,checkMode,lightingEffectSwitch[0],tempScaleVec,glm::vec3(7.0f,4.0f,1.0f), 1.57f, glm::vec3(0.0f,0.0f,1.0f),  32.0f);
    plain->render(ProgramID3,0.1f,checkMode,lightingEffectSwitch[0],tempScaleVec,glm::vec3(7.0f,4.0f,3.0f), 1.57f, glm::vec3(0.0f,0.0f,1.0f),  32.0f);

    plain->render(ProgramID3,0.2f,checkMode,lightingEffectSwitch[0],tempScaleVec,glm::vec3(2.0f,2.0f,1.0f), 1.57f, glm::vec3(0.0f,0.0f,1.0f),  32.0f);
    plain->render(ProgramID3,0.2f,checkMode,lightingEffectSwitch[0],tempScaleVec,glm::vec3(2.0f,2.0f,-1.0f), 1.57f, glm::vec3(0.0f,0.0f,1.0f),  32.0f);

    // bottom
    plain->render(ProgramID3,0.4f,checkMode,lightingEffectSwitch[0],tempScaleVec,glm::vec3(-2.0f,0.0f,0.0f), 0.0f, glm::vec3(0.0f,0.0f,1.0f),  32.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TexID[1]);
    glUniform1i(textureHandle0, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, TexID2[3]);
    glUniform1i(textureHandle1, 1);

    // 2nd room back wall
    plain->render(ProgramID3,0.4f,checkMode,3.0f,tempScaleVec,glm::vec3(1.0f,3.0f,0.0f), 1.57f, glm::vec3(0.0f,0.0f,1.0f),  32.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TexID[1]);
    glUniform1i(textureHandle0, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, TexID2[4]);
    glUniform1i(textureHandle1, 1);

    // 2nd room top wall
    plain->render(ProgramID3,0.4f,checkMode,2.0f,tempScaleVec,glm::vec3(-2.0f,2.0f,0.0f), 0.0f, glm::vec3(0.0f,0.0f,1.0f),  32.0f);
    int waveChangeHandle = glGetUniformLocation(ProgramID3,"wave");

    wave += 0.01;
    glUniform1f(waveChangeHandle,wave);

    // house wall light properties
    // lamp 1 light 1-3
    glm::vec3 HouseWallLightpos = glm::vec3(0.0f,0.143f,0.0f);
    glm::vec3 HouseWallLightpos2 = glm::vec3(-0.719994f,0.142f,0.0f);
    glm::vec3 HouseWallLightpos3 = glm::vec3(-0.719994f,0.142f,0.0f);
    glm::vec3 HouseWallLightpos4 = glm::vec3( -0.899992f,0.142f,0.0f);


    std::vector<glm::vec3> LPos2;
    LPos2.push_back(HouseWallLightpos);
    LPos2.push_back(HouseWallLightpos2);
    LPos2.push_back(HouseWallLightpos3);
    LPos2.push_back(HouseWallLightpos4);
    bulb2->lightRender(LPos2);


    // draw end
    glBindVertexArray(0);
    glutSwapBuffers();
    glFlush();

}


/**
 * @brief DisplayFunc
 */
void DisplayFunc()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // Update the camera, and draw the scene.
    Camera->update(Input, moveForward, moveLeft);
    // draw scene
    draw();
    // stop movement when release
    Input.deltaX = Input.deltaY = 0;  
    moveForward = 0;
    moveLeft = 0;
}


/**
 * Called while a keyboard key press is detected
 * @param key, the keyboard key that made the event
 * @param x, not used
 * @param y, not used
 */

void SpecialFunc( int key, int x, int y )
{
   	switch(key) 
	{
        case GLUT_KEY_LEFT:     
            break;
        case GLUT_KEY_RIGHT:       
            break;
        case GLUT_KEY_UP:           
            break;
        case GLUT_KEY_DOWN:
            break;
    }
    glutPostRedisplay();
}

/**
 * @brief KeyboardFunc
 * @param key
 * @param x
 * @param y
 */
void KeyboardFunc(unsigned char key, int x, int y) 
{
    switch(key)
    {
    case 't': // toggle wireframe or texture mode
        TogglePolygon();
        if(checkMode == true)
        {
            cout<<endl;
            cout<<"Wireframe mode enable..."<<endl;
            checkMode = false;
        }
        else if(checkMode == false)
        {
            cout<<endl;
            cout<<"Shaded mode enable..."<<endl;
            checkMode = true;
        }
        break;
    case 'l': // toggle 3 light mode
        break;
    case 27: // escape key pressed
        exit(0);
    case 'w':
        if(camMode == 2)
        {
            moveForward += 0.1f;
            Camera->MoveCamera(moveForward);
        }
        else{
            moveForward += 0.01f;
        }
        break;
    case 's': 
        if(camMode == 2)
        {
            moveForward -= 0.1f;
            Camera->MoveCamera(moveForward);
        }
        else{
            moveForward -= 0.01f;
        }
        break;
    case 'a':     
        if(camMode == 2)
        {
            moveLeft += 0.1f;
            Camera->RotateAroundPoint(robotPos,moveLeft,0,1,0);
        }
        else{
            moveLeft += 0.01f;
        }
        break;
    case 'd':
        if(camMode == 2)
        {
            moveLeft -= 0.1f;
            Camera->RotateAroundPoint(robotPos,moveLeft,0,1,0);
        }
        else{
            moveLeft -= 0.01f;
        }
        break;
    case '1':
        cout<<"1st Person Mode Enable: You can free go in 3D space."<<endl;
        Camera = ObjCam;
        camMode = 1;
        break;
    case '2':
        cout<<"3rd Person Mode Enable: You can free go in 2D space."<<endl;
        Camera = TPCam;
        camMode = 2;
        robotPos = glm::vec3(0.0f, 0.0f, 0.0f);
        break;
    case 'u':
        lightForward -= 0.001f;
        break;
    case 'j':
        lightForward += 0.001f;
        break;
    case 'h':
        lightLeft -= 0.001f;
        break;
    case 'k':
        lightLeft += 0.001f;
        break;
    case 'g':
        lightup += 0.001f;
        break;
    case 'f':
        lightup -= 0.001f;
        break;

    }
    glutPostRedisplay();
}

/**
 * Program entry. Sets up OpenGL state, GLSL Shaders and GLUT window and function call backs
 */
int main(int argc, char **argv) {

	// Set up GLUT window
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 0);
    winW = 1280;
    winH = 720;
	glutInitWindowSize(winW, winH);

#ifdef __APPLE__
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE | GLUT_STENCIL );
#else
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL );
#endif
    glutCreateWindow("Assign3: SKY GLASS KITCHEN");

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}    
	
    GLenum err = glGetError();
    if (err != 0) {
        fprintf(stderr, "GLEW initialisation error %d\n", err);
    }
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    //glDepthFunc(GL_LEQUAL);
    
    ProgramID = LoadShaders("shaded.vert", "shaded.frag");
	if (ProgramID == 0) {
        fprintf(stderr, "Error loading shader1\n");
        exit(1);
    }

    ProgramID2 = LoadShaders("wireframe.vert", "wireframe.frag");
    if (ProgramID2 == 0){
        fprintf(stderr, "Error loading shader2\n");
        exit(1);
    }

    ProgramID3 = LoadShaders("Bump.vert", "Bump.frag");
    if (ProgramID3 == 0){
        fprintf(stderr, "Error loading shader3\n");
        exit(1);
    }

    ProgramID4 = LoadShaders("sky.vert","sky.frag");
    if (ProgramID4 == 0){
        fprintf(stderr, "Error loading shader4\n");
        exit(1);
    }

    ProgramID5 = LoadShaders("SkyBox.vert","SkyBox.frag");
    if (ProgramID5 == 0){
        fprintf(stderr, "Error loading shader5\n");
        exit(1);
    }

    ProgramID6 = LoadShaders("Mirror.vert","Mirror.frag");
    if (ProgramID6 == 0){
        fprintf(stderr, "Error loading shader6\n");
        exit(1);
    }



    glUseProgram(ProgramID);

    //load data from file
    useTinyOBJLoader();
    cout<<"Total loaded objects: "<<allModel.size()<<endl;
    cout<<"Total loaded object MTL files: "<<allMTL.size()<<endl;
    cout<<"Loading images ......"<<endl;

    //lighting effect initialise to off
    lightingEffectSwitch[0] = 0;

	if (setup() !=0) {
        fprintf(stderr, "Error loading textures' images\n");
		return 1;
    }

    cout<<"Loading Success..."<<endl;
    std::cout <<endl;
    std::cout << "Controls: "<<endl;
    std::cout << " - Press and drag mouse left button to rotate, press 'W', 'S', 'A', 'D' to move." <<endl;
    std::cout << " - Press 't' switch between Wireframe or Texture mode." <<endl;
    std::cout << " - Press '1' and '2' switch between 1st Person mode and 3rd Person mode."<<endl;
    std::cout << "Enjoy it!"<<endl;

    // non-scaled initial Camera setting
    eye = glm::vec3(0.0f, 0.15f, 0.3f);
    at = glm::vec3(0.0f, 0.07f, 0.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);


    // set Camera
    ObjCam = new ObjectViewer(eye, at, up);
    TPCam = new ThirdPersonViewer(eye,at,up);
    Camera = ObjCam;
    // set robot postion
    robotPos = glm::vec3(0.0f, 0.0f, 0.0f);


    // GLUT callbacks and event loop entry
    glutKeyboardFunc(KeyboardFunc);
    glutSpecialFunc(SpecialFunc);
    glutDisplayFunc(DisplayFunc);
    glutReshapeFunc(ReshapeFunc);
    glutTimerFunc( 33, TimerFunc, 0 );
    glutMouseFunc(MouseFunc);
    glutMotionFunc(MotionFunc);

	glutMainLoop();

	return 0;
}
