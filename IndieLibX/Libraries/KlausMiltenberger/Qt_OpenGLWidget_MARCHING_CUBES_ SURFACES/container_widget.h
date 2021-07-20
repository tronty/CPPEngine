/*
 * container_widget.h
 *  Last Changes: 06.05.2018
 *  Author: manitoo  Qt/C++ Coding
 */

#ifndef CONTAINER_WIDGET_H
#define CONTAINER_WIDGET_H

#include <QBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QRadioButton>
#include <QTextEdit>
#include <QLabel>

#include "gl_widget.h"
#include "clockwidget.h"

class Container_Widget : public QWidget
{
    Q_OBJECT

public:
    Container_Widget(QWidget *parent = nullptr, GLbyte Resolution = 100);
    ~Container_Widget();

    ClockWidget *clock_widget;
    GL_Widget   *gl_widget;
    QTextEdit   *textedit;

private:
    //Avoid Default-Assignment-/Copy-Constructor
    Container_Widget();
    Container_Widget(const Container_Widget&);
    Container_Widget& operator=(const Container_Widget&);
};

#endif // CONTAINER_WIDGET_H
