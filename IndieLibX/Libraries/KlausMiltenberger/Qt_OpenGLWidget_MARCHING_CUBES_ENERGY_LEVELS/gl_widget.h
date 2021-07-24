/*
 *  gl_widget.h
 *  Last Change: 06.05.2018
 *  Math. Reference: Paul Bourke, Marching Cubes Algorithm
 *  Author: manitoo Qt/C++ , QOpenGLWidget Class
 */

#ifndef GL_WIDGET_H
#define GL_WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderprogram>
#include <QMouseEvent>

#include <boost/math/special_functions.hpp>

/* (Detailed Calls !)
#include <usr/local/include/boost/math/special_functions/bessel.hpp>
#include <usr/local/include/boost/math/special_functions/laguerre.hpp>
#include <usr/local/include/boost/math/special_functions/spherical_harmonic.hpp>
#include <usr/local/include/boost/math/complex.hpp>
*/

#include "marching_cubes_data.h"

using namespace boost::math;

// ---- [ Begin: GL_Widget | QGLWidget(deprecated) renders offscreen, in framebuffer objects ] -----------
class GL_Widget : public QOpenGLWidget , protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GL_Widget(QWidget *parent = nullptr, GLuint Resolution = 51);
    ~GL_Widget();

    GLuint    iDataSetSize;
    GLfloat   fStepSize;

protected:
        void initializeGL(void) override;
        void resizeGL(int,int) override;
        void paintGL(void) override;

        void mousePressEvent(QMouseEvent*) override;
        void mouseMoveEvent(QMouseEvent*) override;
        void wheelEvent(QWheelEvent*) override;
        void mouseDoubleClickEvent(QMouseEvent*) override;
        void timerEvent(QTimerEvent*) override;

public slots:
    void Level_Set(GLuint);
    void Partition_Mode(GLuint);

    //GL Actions
    void animate(void);
    void shading(void);
    void blending(void);
    void culling(void);
    void projection(void);

    //Level Set Actions
    void LS1(void)  {Level_Set(1); update();}
    void LS2(void)  {Level_Set(2); update();}
    void LS3(void)  {Level_Set(3); update();}
    void LS4(void)  {Level_Set(4); update();}

    //Tesselation Actions
    void CUBES(void)        {Partition_Mode(1); update();}
    void TETRAHEDRONS(void) {Partition_Mode(2); update();}

private:
    struct GLvector {GLfloat fX , fY , fZ;};

    //Marching Cubes Algorithm
    GLvoid vMarchingCubes(GLint potential, GLint partition);
    GLvoid vMarchCube1(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ, const GLfloat &fScale, const GLfloat &fTv);
    GLvoid vMarchCube2(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ, const GLfloat &fScale, const GLfloat &fTv);
    GLvoid vMarchTetrahedron(GLvector *pasTetrahedronPosition, GLfloat *pafTetrahedronValue, GLfloat fTv);

    //Function Pointers
    void (GL_Widget::*vMarchCube)(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ, const GLfloat &fScale, const GLfloat &fTv);
    GLfloat (GL_Widget::*fSample)(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ);

    //Helpers
    GLfloat fGetOffset(const GLfloat &fValue1, const GLfloat &fValue2, const GLfloat &fValueDesired);
    void vNormalizeVector(GLvector &rfVectorResult, GLvector &rfVectorSource);
    void vGetNormal(GLvector &rfNormal, const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ);

    void axes(const GLfloat &x, const GLfloat &y, const GLfloat &z);

    //Level-Set Samples
    GLfloat fSample1(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ);
    GLfloat fSample2(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ);
    GLfloat fSample3(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ);
    GLfloat fSample4(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ);

    void zoom(double zoomFactor);

    //GUI Control Variables
    GLubyte   LEVEL_SET;
    GLubyte   PARTITION_MODE;
    GLubyte   POLYGON_MODE;
    GLubyte   PROJECTION_MODE;
    GLubyte   BLENDING_MODE;
    GLubyte   CULLING_MODE;

    //Mouse Position
    QPoint      lastPos;

    //Rotation Angles
    GLfloat     rotX, rotY, rotZ;

    //Wheel Scaling
    GLdouble    currScaling;
    GLdouble    zoomFactor;

    //Timer
    GLbyte	timer_Id;
    GLfloat	t;

    //Display List IDs
    GLuint  Dodecahedron,
            Level_Set_11, Level_Set_12,
            Level_Set_21, Level_Set_22,
            Level_Set_31, Level_Set_32,
            Level_Set_41, Level_Set_42;

    //Shader-Program
    QGLShaderProgram    *program;

    //Uniform Locations
    GLint   specLevelLocation;
    GLint   specSizeLocation;

    //Uniform Locations
    GLint   colorLocation[4];
    GLint   lightLocation[3];

    //Avoid Default-/Assign-Constructor
    GL_Widget();
    GL_Widget(const GL_Widget&);
    GL_Widget& operator=(const GL_Widget&);
};
// ---- [ End:  GL_Widget ] ---------------------------

#endif // GL_WIDGET_H
