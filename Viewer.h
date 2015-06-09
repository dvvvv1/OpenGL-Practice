//Author: Puzhi Yao
//Date: 18 May 2015
//Program description:
//CG Assignment 3 SKY GLASS KITCHEN

/**
 Some viewer classes. All viewers inherit from a base Viewer class.
 See model-view.cpp for example usage.
 Anthony Dick
*/

#ifndef _VIEWER_H_
#define _VIEWER_H_

#include <vector>
#include "InputState.h"
#include "glm/glm.hpp"

/**
 The base Viewer class stores a current view transformation.
 It is initialised using the same parameters as gluLookAt.
 It is updated in response to user input or it can be reset.
 Method orthogonaliseViewMtx() can be used to correct drift if needed.
*/
class Viewer 
{
protected:
    glm::mat4 viewMtx;
    glm::vec3 initEye;
    glm::vec3 initAt;
    glm::vec3 initUp;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 robotPos;
    glm::vec3 m_vPosition;
    glm::vec3 m_vView;
    glm::vec3 m_vUpVector;
    int FOV;

public:
    Viewer( glm::vec3 eye, glm::vec3 at, glm::vec3 up);
    virtual ~Viewer() = 0;

    const glm::mat4 getViewMtx() const;
    //    void orthogonaliseViewMtx();
    void reset();

    virtual void update( InputState input, float forward, float moveLeft ) = 0;
    virtual int getFOV() = 0;
    virtual void setViewMtx(glm::mat4 viewMtrix) = 0;
    virtual glm::vec3 getCamPos() = 0;
    virtual void setRobotPos(glm::vec3 tempPos) = 0;
    virtual void PositionCamera(float positionX, float positionY, float positionZ,
                                float viewX, float viewY, float viewZ,
                                float upVectorX, float upVectorY, float upVectorZ) = 0;
    virtual void RotateView(float angle, float x, float y, float z) = 0;
    virtual void RotateAroundPoint(glm::vec3 vCenter, float angle, float x, float y, float z) = 0;
    virtual void MoveCamera(float speed) = 0;
    virtual glm::vec3 getRobotPos() = 0;

};


/**
 ObjectViewer rotates about current camera x and y axes in
 response to mouse motion. To do this it reads the current 
 camera axes from the viewing matrix.
*/
class ObjectViewer : public Viewer
{
public:
  ObjectViewer( glm::vec3 eye, glm::vec3 at, glm::vec3 up);

    virtual void update(InputState input , float forward, float moveLeft);
    virtual int getFOV();
    virtual void setViewMtx(glm::mat4 viewMtrix);
    virtual glm::vec3 getCamPos();
    virtual void setRobotPos(glm::vec3 tempPos);
  virtual void PositionCamera(float positionX, float positionY, float positionZ,
                              float viewX, float viewY, float viewZ,
                              float upVectorX, float upVectorY, float upVectorZ);
  virtual void RotateView(float angle, float x, float y, float z);
  virtual void RotateAroundPoint(glm::vec3 vCenter, float angle, float x, float y, float z);
  virtual void MoveCamera(float speed);
  virtual glm::vec3 getRobotPos();

};

/**
 * @brief The ThirdPersonViewer class
 * rotate around object
 */
class ThirdPersonViewer: public Viewer
{
public:
    ThirdPersonViewer( glm::vec3 eye, glm::vec3 at, glm::vec3 up);
    virtual void update(InputState input , float forward, float moveLeft);
    virtual int getFOV();
    virtual void setViewMtx(glm::mat4 viewMtrix);
    virtual glm::vec3 getCamPos();
    virtual void setRobotPos(glm::vec3 tempPos);
    virtual void PositionCamera(float positionX, float positionY, float positionZ,
                                float viewX, float viewY, float viewZ,
                                float upVectorX, float upVectorY, float upVectorZ);
    virtual void RotateView(float angle, float x, float y, float z);
    virtual void RotateAroundPoint(glm::vec3 vCenter, float angle, float x, float y, float z);
    virtual void MoveCamera(float speed);
    virtual glm::vec3 getRobotPos();
};


#endif // VIEWER_H
