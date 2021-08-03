/*
 * gl_widget.h
 *  Last Changes: 06.05.2018
 *  Math. Code Reference: Paul Bourke , Marching-Cubes-Algorithm
 *  Author: manitoo  Qt/OpenGL/C++ Coding
 *  QOpenGLWidget , QOpenGLFunctions Implementation
 */

#ifndef GL_WIDGET_H
#define GL_WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderprogram>
#include <QMouseEvent>

#include "marching_cubes_data.h"
#include "marching_cubes_algm.h"

class GL_Widget : public QOpenGLWidget , protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GL_Widget(GLbyte resolution = 100);
    ~GL_Widget();

    GLubyte   iDataSetSize;
    GLfloat   fStepSize;

protected:
        //Reimplemented Virtual Functions
        void initializeGL(void) override;
        void resizeGL(int,int) override;
        void paintGL(void) override;

        //Event Handlers
        void mousePressEvent(QMouseEvent*) override;
        void mouseMoveEvent(QMouseEvent*) override;
        void wheelEvent(QWheelEvent*) override;
        void mouseDoubleClickEvent(QMouseEvent*) override;
        void timerEvent(QTimerEvent*) override;

public slots:
    void Level_Set(GLbyte);
    void Partition_Mode(GLbyte);

    //GL Actions
    void animate(void);
    void shading(void);
    void blending(void);
    void culling(void);
    void projection(void);

    //Level Set Actions , inline
    void LS1(void)  {Level_Set(1); update();}
    void LS2(void)  {Level_Set(2); update();}
    void LS3(void)  {Level_Set(3); update();}
    void LS4(void)  {Level_Set(4); update();}
    void LS5(void)  {Level_Set(5); update();}

    //Partition Mode Actions , inline
    void CUBES(void)        {Partition_Mode(1); update();}
    void TETRAHEDRONS(void) {Partition_Mode(2); update();}


private:
    void axes(const GLfloat &x, const GLfloat &y, const GLfloat &z);
    void zoom(const GLfloat zoomFactor);

    //GUI Controls
    GLbyte   LEVEL_SET;
    GLbyte   PARTITION_MODE;
    GLbyte   POLYGON_MODE;
    GLbyte   PROJECTION_MODE;
    GLbyte   BLENDING_MODE;
    GLbyte   CULLING_MODE;

    //Mouse Position
    QPoint      lastPos;

    //Rotation Angles
    GLfloat     rotX, rotY, rotZ;

    //Wheel Scaling
    GLfloat    currScaling;
    GLfloat    zoomFactor;

    //Timer
    GLbyte	timer_Id;
    GLfloat	t, dt;

    //Elapsed Time
    QTime       qtime;

    //Display List IDs (Cubes , Tetrahedrons)
    GLbyte  Dodecahedron,
            Level_Set_11, Level_Set_12,
            Level_Set_21, Level_Set_22,
            Level_Set_31, Level_Set_32,
            Level_Set_41, Level_Set_42,
            Level_Set_51, Level_Set_52;

    //Marching-Cubes-Algorithm-Class
    MC_ALG *mca;

    //Shader-Program
    QOpenGLShaderProgram    *program;

    //Uniform Locations
    GLbyte   colorLocation[4];
    GLbyte   lightLocation[3];

    //Avoid Default-/Assignment-/Copy-Constructor
    GL_Widget();
    GL_Widget(const GL_Widget&);
    GL_Widget& operator=(const GL_Widget&);
};

#endif // GL_WIDGET_H
