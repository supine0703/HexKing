#include "HexTimer.h"

#include <QTime>

#define DAYTIME    86400000
#define HOURTIME   3600000
#define MINUTETIME 60000
#define SECONDTIME 1000

HexTimer::HexTimer()
    : isStart(0)
    , startTime(0)
{ }

HexTimer::~HexTimer()
{ }

void HexTimer::Start()
{
    if (!isStart)
    {
        isStart = true;
        startTime = QTime::currentTime().msecsSinceStartOfDay() - startTime;
    }
}

void HexTimer::Reset()
{
    isStart = 0;
    startTime = 0;
}

void HexTimer::Stop()
{
    if (isStart)
    {
        startTime = this->Msec();
        isStart = false;
    }
}

int HexTimer::Msec()
{
    if (isStart)
    {
        int buff = QTime::currentTime().msecsSinceStartOfDay() - startTime;
        return buff < 0 ? buff + DAYTIME : buff;
    }
    else return startTime;
}

QString HexTimer::Str()
{
    int time = this->Msec() + 10;
    int h  = time / HOURTIME;
    int m  = (time % HOURTIME) / MINUTETIME;
    int s  = (time % MINUTETIME) / SECONDTIME;
    int ms = (time % SECONDTIME) / 100;
    return (
        QString("")
        + (h ? QString("%1").arg(h, 2, 10, QChar('0')) + " : " : "")
        + (h || m ? QString("%1").arg(m, 2, 10, QChar('0')) + " : " : "")
        + (QString("%1").arg(s, 2, 10, QChar('0')) + "." + QString::number(ms))
    );
}

