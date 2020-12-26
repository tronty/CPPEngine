/*
 * main.cpp
 *  Math. Code Reference: Marching Cubes C-Algorithmus by Paul Bourke
 *  Last Changes: 06.05.2018
 *  Author: manitoo , C++/Qt  Subclassing QOpenGLWidget , QOpenGLFunctions , GLSL Shading
 */

#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setFont(QFont("Palatino", 12, QFont::Bold));

    // --- An OpenGL Rendering Context is a set of OpenGL State Variables ---
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setVersion(3, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);
  //format.setProfile(QSurfaceFormat::CompatibilityProfile);
    QSurfaceFormat::setDefaultFormat(format);

    MainWindow mainwindow(nullptr,/* Resolution <= 101 */51);
    mainwindow.setWindowTitle("[  Marching Cubes | Cube/Tetrahedron - Algorithm | Math-Level-Sets  ]");

    //Geometry (Resolution)
    mainwindow.setGeometry(10,40,2500,1320);
    mainwindow.show();

    //Event Loop
    return app.exec();
}
