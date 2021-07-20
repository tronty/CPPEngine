/*
 * container_widget.h
 *  Last Change: 06.05.2018
 *  Author: manitoo
 */

#ifndef CONTAINER_WIDGET_H
#define CONTAINER_WIDGET_H

#include "clock_widget.h"
#include "gl_widget.h"

class Container_Widget : public QWidget
{
    Q_OBJECT

public:
    Container_Widget(QWidget *parent = nullptr, GLuint Resolution = 100);
    ~Container_Widget();

    GL_Widget   *glwidget;
    Clock_Widget *clockwidget;
    QTextEdit   *textedit;

private:
    //Avoid Default-Assign-Copy-Constructor
    Container_Widget();
    Container_Widget(const Container_Widget&);
    Container_Widget& operator=(const Container_Widget&);
};

#endif // CONTAINER_WIDGET_H
