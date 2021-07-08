/*
 * mainwindow.cpp
 *  Last Change: 06.05.2018
 *  Author: manitoo  Qt/C++
 */

#include "mainwindow.h"
#include "gl_widget.h"

MainWindow::MainWindow(QWidget *parent, GLuint Resolution) : QMainWindow(parent)
{
    qDebug() << "  [ MainWindow constructed ]";

    cnt_widget       = new Container_Widget(0,Resolution);

    setCentralWidget(cnt_widget);

    statuslabel = new QLabel();
    statusBar();
    update_statusBar();
}

MainWindow::~MainWindow()
{
    delete cnt_widget;
    qDebug() << "  [ MainWindow destructed ]\n";
}

void MainWindow::update_statusBar()
{
    statuslabel->setText("[  Space Scanning   Energy-Level-Sets  |  GLSL 1.20  |  OpenGL 2.1   Subclassing  QGLWidget  |  AMD Radeon Pro Vega 56 OpenGL Engine  ]");
    statuslabel->setAlignment(Qt::AlignHCenter);
    statuslabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
    statusBar()->addWidget(statuslabel,1);
}

