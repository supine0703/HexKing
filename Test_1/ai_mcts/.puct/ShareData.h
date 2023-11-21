#ifndef SHAREDATA_H
#define SHAREDATA_H

#include <QVector>

#define hisMove HisMoveVec::getHM
#define his HisMoveVec::getH
#define rdy HisMoveVec::ready
#define wner HisMoveVec::getWner

class HisMoveVec
{
public:
    static bool& getWner()
    {
        static bool data;
        return data;
    }

    static QVector<int>& getHM()
    {
        static QVector<int> data(0);
        return data;
    }

    static QVector<int>& getH()
    {
        static QVector<int> data(0);
        return data;
    }

    static bool& ready()
    {
        static bool data = false;
        return data;
    }

private:
    HisMoveVec() { }
    ~HisMoveVec() { }
};

#endif // SHAREDATA_H
