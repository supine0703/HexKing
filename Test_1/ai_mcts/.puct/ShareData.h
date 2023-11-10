#ifndef SHAREDATA_H
#define SHAREDATA_H

#include <QVector>

#define hisMove HisMoveVec::getHM
#define his HisMoveVec::getH

class HisMoveVec
{
public:
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

private:
    HisMoveVec() { }
    ~HisMoveVec() { }
};

#endif // SHAREDATA_H
