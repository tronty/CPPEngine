#-------------------------------------------------
#
# Project created by QtCreator 2011-12-14T18:57:25
#
#-------------------------------------------------

QT          += opengl
CONFIG      += no_lflags_merge


#macport boost
INCLUDEPATH     += /opt/local/include /usr/local/include

#Not Used
#LIBS            += -L/opt/local/lib/ -lboost_thread-mt

TARGET      = Qt_OpenGLWidget_MARCHING_CUBES_ENERGY_LEVELS
TEMPLATE    = app

SOURCES += main.cpp\
        mainwindow.cpp \
        container_widget.cpp \
        gl_widget.cpp \
        clock_widget.cpp

HEADERS += mainwindow.h \
        container_widget.h \
        gl_widget.h \
        marching_cubes_data.h \
        clock_widget.h

RESOURCES += \
    images.qrc









