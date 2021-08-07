/*
 * mainwindow.cpp
 *  Last Changes: 06.05.2018
 *  Author: manitoo  Qt/C++ Coding
 */

#include "mainwindow.h"
#include "gl_widget.h"

//Constructor
MainWindow::MainWindow(QWidget *parent, short Resolution) : QMainWindow(parent)
{
    qDebug() << "  [ Mainwindow       constructed   | Main-TID: " << QThread::currentThreadId() << " ]";

    cnt_widget       = new Container_Widget(0,Resolution);

    setCentralWidget(cnt_widget);

    statuslabel = new QLabel();
    statusBar();
    update_statusBar();
}

//Destructor
MainWindow::~MainWindow()
{
    delete cnt_widget;
    delete statuslabel;
    qDebug() << "  [ MainWindow          destructed ]";
}

void MainWindow::update_statusBar()
{
    statuslabel->setText("[ Space Scanning  |  Math-Level-Sets  |  GLSL 1.20  |  OpenGL 2.1  Subclassing  QOpenGLWidget & QOpenGLFunctions  |  AMD Radeon Pro Vega 56 OpenGL Engine ]");
    statuslabel->setAlignment(Qt::AlignHCenter);
    statuslabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
    statusBar()->addWidget(statuslabel,1);
}
