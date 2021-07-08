# -------------------------------------------------
# Project created by QtCreator 2016-07-07T02:34:27
# -------------------------------------------------

QT       += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = Qt_QOpenGLWidget_MARCHING_CUBES_SURFACES
TEMPLATE    = app

SOURCES += main.cpp \
    mainwindow.cpp \
    gl_widget.cpp \
    container_widget.cpp \
    clockwidget.cpp \
    marching_cubes_algm.cpp

HEADERS += mainwindow.h \
    gl_widget.h \
    container_widget.h \
    clockwidget.h \
    marching_cubes_data.h \
    marching_cubes_algm.h

RESOURCES += \
    images.qrc










