#include <QtGui>
#include "clock_widget.h"

Clock_Widget::Clock_Widget(QWidget *parent)
  : QLCDNumber(parent), showClock(true)
{
  updateTimer = startTimer(1000);
  switchTimer = startTimer(1000);

  QTimerEvent *e = new QTimerEvent(updateTimer);
  QCoreApplication::postEvent(this, e);
}

void Clock_Widget::timerEvent(QTimerEvent *e)
{
  if (!e) return;

  if (e->timerId() == switchTimer)
    showClock = !showClock;

  if (e->timerId() == updateTimer) {
    if (showClock) {
      QTime time = QTime::currentTime();
      QString str = time.toString(Qt::LocalDate);
      display(str);
    } else {
      QDate date = QDate::currentDate();
      QString str = date.toString(Qt::LocalDate);
      display(str);
    }
  }
}

