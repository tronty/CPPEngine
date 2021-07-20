/*
 * mainwindow.h
 *  Last Change: 06.05.2018
 *  Author: manitoo  Qt/C++
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStatusBar>
#include <QLabel>

#include "container_widget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr, GLuint Resolution = 51);
    ~MainWindow();

private:
    void update_statusBar(void);

    Container_Widget    *cnt_widget;    //--> Central Widget
    QLabel              *statuslabel;   //--> Status Text Label

    //Avoid Default-/Assign-Constructor
    MainWindow();
    MainWindow& operator=(const MainWindow&);
};

#endif // MAINWINDOW_H
