/*
 * mainwindow.h
 *  Last Changes: 06.05.2018
 *  Author: manitoo  Qt/C++ Coding
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStatusBar>

#include "container_widget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr, short Resolution = 101);
    ~MainWindow();

private:
    void update_statusBar(void);

    Container_Widget    *cnt_widget;
    QLabel              *statuslabel;

    //Avoid Default-Assigment-/Copy-Constructor
    MainWindow();
    MainWindow(const MainWindow&);
    MainWindow& operator=(const MainWindow&);
};

#endif // MAINWINDOW_H
