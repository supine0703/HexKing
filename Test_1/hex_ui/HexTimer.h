#ifndef HEXTIMER_H
#define HEXTIMER_H

#include <QString>

class HexTimer
{
public:
    HexTimer();
    ~HexTimer();

    void Start();
    void Reset();
    void Stop();
    int Msec();
    QString Str();

private:
    int startTime;
    bool isStart;
};

#endif // HEXTIMER_H
