/*********************************************************
 Version 1.000
 
 Code provided by Michael Hemsley and Anthony Dick
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
//
#ifndef SPHERE_H
#define SPHERE_H

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
using namespace std;

class Sphere {
public:

	/**
	 * Generates a new sphere mesh with given radius, vertical sub-divisions and horizontal
	 * sub-divisions. Produces vertex positions, normal vectors, and element
	 * indices available for use in arrays.
	 * More sub-divisions generates a smoother sphere.
	 * It is a good idea to have vertical and horizontal divisions equal.
	 * @param radius, radius of sphere in generic units
	 * @param vertDiv, (min 1), number of vertical divisions (bands)
	 * @param horzDiv, (min 4), number of horizontal divisions (slices)
	 */
	Sphere(float radius = 1.0f, int vertDiv = 16, int horzDiv = 16);

	float *vertices;		// Vertex position (x,y,z)
	float *normals;			// Normal vector (x,y,z)
    float *texCoords;		// TexCoord (ST)
	unsigned int *indices;	// Element indices
    float *colours;

	// Counts of array elements
	int vertCount;
	int normCount;
	int texCount;
	int indCount;
    int colorCount;

    std::vector<float> autoCalNormals(std::vector<float> v1, std::vector<float> v2, std::vector<float> v3);
    std::vector<int> findRelativeVertex(int index);


};

#endif
