//Author: Puzhi Yao
//Date: 18 May 2015
//Program description:
//CG Assignment 3 SKY GLASS KITCHEN


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>

#include "Viewer.h"
#include "glm/gtc/matrix_transform.hpp"

#define DEG2RAD(x) ((x)*M_PI/180.0) 
#define RAD2DEG(x) ((x)*180.0/M_PI) 

// Initial position : on +Z
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;

Viewer::Viewer( glm::vec3 eye, glm::vec3 at, glm::vec3 up)
{
    initEye = eye;
    initAt = at;
    initUp = up;
    position = glm::vec3( -0.02, 0.07f, 0.0f );
    reset();
}

Viewer::~Viewer(){}

/**
 Assumes the current matrix has been calculated (usually in 
 update() or reset())
*/
const glm::mat4 Viewer::getViewMtx() const
{
    return viewMtx;
}

/**
 Resets the view matrix to the value the camera was 
 initialised with
*/
void Viewer::reset()
{
    viewMtx = glm::lookAt(initEye, initAt, initUp);
}

ObjectViewer::ObjectViewer(glm::vec3 eye, glm::vec3 at, glm::vec3 up)
: Viewer(eye, at, up)
{
}

void ObjectViewer::update( InputState input, float forward, float moveLeft )
{
    float xRot = -input.deltaX;
    float yRot = -input.deltaY;

    // Compute new orientation
    horizontalAngle += xRot/500;
    verticalAngle += yRot/500;

    // direction
    glm::vec3 direction(
                cos(verticalAngle) * sin(horizontalAngle),
                sin(verticalAngle),
                cos(verticalAngle) * cos(horizontalAngle)
                );
    // Right vector
    glm::vec3 right = glm::vec3(
                sin(horizontalAngle - 3.14f/2.0f),
                0,
                cos(horizontalAngle - 3.14f/2.0f)
            );
    // up vector
    glm::vec3 up = glm::cross(right, direction);

    position += direction * forward - right * moveLeft;

    viewMtx = glm::lookAt(
                position,
                position+direction,
                up);
    robotPos = position+direction;

}

int ObjectViewer::getFOV()
{
    return FOV;
}

void ObjectViewer::setViewMtx(glm::mat4 viewMtrix)
{
    viewMtx = glm::mat4(viewMtrix);
}

glm::vec3 ObjectViewer::getCamPos()
{
    return position;
}

void ObjectViewer::setRobotPos(glm::vec3 tempPos)
{
    this->robotPos = tempPos;
}

void ObjectViewer::PositionCamera(float positionX, float positionY, float positionZ,
                            float viewX, float viewY, float viewZ,
                            float upVectorX, float upVectorY, float upVectorZ){}
void ObjectViewer::RotateView(float angle, float x, float y, float z){}
void ObjectViewer::RotateAroundPoint(glm::vec3 vCenter, float angle, float x, float y, float z){}
void ObjectViewer::MoveCamera(float speed){}
glm::vec3 ObjectViewer::getRobotPos(){}

ThirdPersonViewer::ThirdPersonViewer(glm::vec3 eye, glm::vec3 at, glm::vec3 up): Viewer(eye,at,up)
{
    m_vPosition = eye;
    m_vView = at;
    m_vUpVector = up;
}

void ThirdPersonViewer::update( InputState input, float forward, float moveLeft )
{
    viewMtx = glm::lookAt(m_vPosition,m_vView,m_vUpVector);
    setRobotPos(m_vView);
}

int ThirdPersonViewer::getFOV()
{
    return FOV;
}

void ThirdPersonViewer::setViewMtx(glm::mat4 viewMtrix)
{
    viewMtx = glm::mat4(viewMtrix);
}

glm::vec3 ThirdPersonViewer::getCamPos()
{
    return glm::vec3(viewMtx[1][0],viewMtx[1][1],viewMtx[1][2]);
}

void ThirdPersonViewer::setRobotPos(glm::vec3 tempPos)
{
    this->robotPos = tempPos;
}

void ThirdPersonViewer::PositionCamera(float positionX, float positionY, float positionZ, float viewX, float viewY, float viewZ, float upVectorX, float upVectorY, float upVectorZ)
{
    m_vPosition = glm::vec3(positionX,positionY,positionZ);
    m_vView = glm::vec3(viewX,viewY,viewZ);
    m_vUpVector = glm::vec3(upVectorX,upVectorY,upVectorZ);
}
void ThirdPersonViewer::RotateView(float angle, float x, float y, float z)
{
    glm::vec3 vNewView;
    glm::vec3 vView = m_vView - m_vPosition;
    float cosTheta = (float)cos(angle);
    float sinTheta = (float)sin(angle);

    vNewView[0]  = (cosTheta + (1 - cosTheta) * x * x)		* vView[0];
    vNewView[0] += ((1 - cosTheta) * x * y - z * sinTheta)	* vView[1];
    vNewView[0] += ((1 - cosTheta) * x * z + y * sinTheta)	* vView[2];

    // Find the new y position for the new rotated point
    vNewView[1]  = ((1 - cosTheta) * x * y + z * sinTheta)	* vView[0];
    vNewView[1] += (cosTheta + (1 - cosTheta) * y * y)		* vView[1];
    vNewView[1] += ((1 - cosTheta) * y * z - x * sinTheta)	* vView[2];

    // Find the new z position for the new rotated point
    vNewView[2]  = ((1 - cosTheta) * x * z - y * sinTheta)	* vView[0];
    vNewView[2] += ((1 - cosTheta) * y * z + x * sinTheta)	* vView[1];
    vNewView[2] += (cosTheta + (1 - cosTheta) * z * z)		* vView[2];

    m_vView = m_vPosition + vNewView;
}

void ThirdPersonViewer::RotateAroundPoint(glm::vec3 vCenter, float angle, float x, float y, float z)
{
    glm::vec3 vNewPosition;

    // To rotate our position around a point, what we need to do is find
    // a vector from our position to the center point we will be rotating around.
    // Once we get this vector, then we rotate it along the specified axis with
    // the specified degree.  Finally the new vector is added center point that we
    // rotated around (vCenter) to become our new position.  That's all it takes.

    // Get the vVector from our position to the center we are rotating around
    glm::vec3 vPos = m_vPosition - vCenter;

    // Calculate the sine and cosine of the angle once
    float cosTheta = (float)cos(angle);
    float sinTheta = (float)sin(angle);

    // Find the new x position for the new rotated point
    vNewPosition[0]  = (cosTheta + (1 - cosTheta) * x * x)		* vPos[0];
    vNewPosition[0] += ((1 - cosTheta) * x * y - z * sinTheta)	* vPos[1];
    vNewPosition[0] += ((1 - cosTheta) * x * z + y * sinTheta)	* vPos[2];

    // Find the new y position for the new rotated point
    vNewPosition[1]  = ((1 - cosTheta) * x * y + z * sinTheta)	* vPos[0];
    vNewPosition[1] += (cosTheta + (1 - cosTheta) * y * y)		* vPos[1];
    vNewPosition[1] += ((1 - cosTheta) * y * z - x * sinTheta)	* vPos[2];

    // Find the new z position for the new rotated point
    vNewPosition[2]  = ((1 - cosTheta) * x * z - y * sinTheta)	* vPos[0];
    vNewPosition[2] += ((1 - cosTheta) * y * z + x * sinTheta)	* vPos[1];
    vNewPosition[2] += (cosTheta + (1 - cosTheta) * z * z)		* vPos[2];

    // Now we just add the newly rotated vector to our position to set
    // our new rotated position of our camera.
    m_vPosition = vCenter + vNewPosition;
}

void ThirdPersonViewer::MoveCamera(float speed)
{
    // Get our view vector (The direciton we are facing)
    glm::vec3 vVector = m_vView - m_vPosition;

    m_vPosition.x += vVector.x * speed;		// Add our acceleration to our position's X
    m_vPosition.z += vVector.z * speed;		// Add our acceleration to our position's Z
    m_vView.x += vVector.x * speed;			// Add our acceleration to our view's X
    m_vView.z += vVector.z * speed;			// Add our acceleration to our view's Z
}
glm::vec3 ThirdPersonViewer::getRobotPos()
{
    return robotPos;
}


