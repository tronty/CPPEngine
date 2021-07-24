/*
 * main.cpp
 *  Last Change: 06.05.2018
 *  Author: manitoo  Qt/C++
 */

#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setFont(QFont("Helvetica", 10, QFont::Bold));

    // --- An OpenGL Rendering Context is a set of OpenGL State Variables ---
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setVersion(3, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);
  //format.setProfile(QSurfaceFormat::CompatibilityProfile);
    QSurfaceFormat::setDefaultFormat(format);

    //MainWindow
    MainWindow mainwindow(0,/* Resolution <= 201 */101);
    mainwindow.setWindowTitle("[ Marching Cubes/Tetrahedrons Algorithm  |  Theoretical Physics Applications ]");
    //Geometry (Resolution)
    mainwindow.setGeometry(10,20,2540,1400);
    mainwindow.show();

    //Event Loop
    return app.exec();
}
