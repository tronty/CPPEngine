/*
 * clock_widget.h
 *  Last Changes: 06.05.2018
 *  Author: manitoo  Qt/C++ Coding
 */

#ifndef CLOCKWIDGET_H
#define CLOCKWIDGET_H

#include <QCoreApplication>
#include <QLCDNumber>
#include <QTime>

class QTimerEvent;

class ClockWidget : public QLCDNumber
{
    Q_OBJECT

public:
    ClockWidget(QWidget *parent = nullptr);

protected:
    void timerEvent(QTimerEvent *);

private:
    int     updateTimer, switchTimer;
    bool    showClock;

    //Avoid Assignment-/Copy-Constructor
    ClockWidget(const ClockWidget&);
    ClockWidget& operator=(const ClockWidget&);
};

#endif // CLOCKWIDGET_H
