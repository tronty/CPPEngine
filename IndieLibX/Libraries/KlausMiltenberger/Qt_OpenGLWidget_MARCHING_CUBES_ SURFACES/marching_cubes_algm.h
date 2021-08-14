/*
 * mc_alg.h
 *  Last Changes: 06.05.2018
 *  Author: manitoo  Qt/C++ Coding
 *  Class MC_ALG encapsulates MARCHING_CUBES_ALGORITHM
 */

#ifndef MARCHING_CUBES_ALGM_H
#define MARCHING_CUBES_ALGM_H

#include <QDebug>
#include <QThread>
#include <QTime>

#include "marching_cubes_data.h"

// ----- [ Begin: MARCHING_CUBES_ALGORITHM ] --------------------------------
class MC_ALG : protected QOpenGLFunctions
{
public:
    MC_ALG(GLint Resolution = 51);
    ~MC_ALG();

    GLvoid vMarchingCubes(GLint potential, GLint partition);

private:

    struct GLvector
    {GLfloat fX, fY, fZ;};

    //Function Pointers
    void    (MC_ALG::*vMarchCube)(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ, const GLfloat &fScale, const GLfloat &fTv);
    GLfloat (MC_ALG::*fSample   )(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ);

    //5 Math-Level-Sets
    GLfloat fSample1(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ);
    GLfloat fSample2(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ);
    GLfloat fSample3(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ);
    GLfloat fSample4(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ);
    GLfloat fSample5(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ);

    //Marching Cubes Algorithms
    GLvoid vMarchCube1(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ, const GLfloat &fScale, const GLfloat &fTv);
    GLvoid vMarchCube2(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ, const GLfloat &fScale, const GLfloat &fTv);
    GLvoid vMarchTetrahedron(GLvector *pasTetrahedronPosition, GLfloat *pafTetrahedronValue, GLfloat fTv);

    //Geometric Calculations
    GLfloat fGetOffset(const GLfloat &fValue1, const GLfloat &fValue2, const GLfloat &fValueDesired);
    GLvoid  vNormalizeVector(GLvector &rfVectorResult, GLvector &rfVectorSource);
    GLvoid  vGetNormal(GLvector &rfNormal, const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ);

    GLbyte    iDataSetSize;
    GLfloat   fStepSize;

    //Avoid Assignment-/Copy-Constructor
    MC_ALG();
    MC_ALG(const MC_ALG&);
    MC_ALG& operator=(const MC_ALG&);
};
// ----- [ End:   MARCHING_CUBES_ALGORITHM ] --------------------------------

#endif // MARCHING_CUBES_ALGM_H
