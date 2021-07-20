#ifndef CLOCKWIDGET_H
#define CLOCKWIDGET_H

#include <QLCDNumber>

class QTimerEvent;

class Clock_Widget : public QLCDNumber
{
    Q_OBJECT

public:
    Clock_Widget(QWidget *parent = nullptr);

protected:
    void timerEvent(QTimerEvent *);

private:
    int     updateTimer, switchTimer;
    bool    showClock;

    //Avoid Assign-/Copy-Constructor
    Clock_Widget(const Clock_Widget&);
    Clock_Widget& operator=(const Clock_Widget&);
};

#endif // CLOCKWIDGET_H
