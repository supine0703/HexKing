#ifndef MOWORK_H
#define MOWORK_H

#include "qtypes.h"
#include "HexMatrix.hpp"

class StoneBoard;
class QElapsedTimer;
struct HexEnvironment;
class HexBoard;

#include "HexColor.hpp"
#include "HexPoint.hpp"

HEXPOINT_HPP

class MoWork
{
public:
    MoWork(const uint& runTime);
    ~MoWork();

    void Work(QElapsedTimer* time, const HexMatrix& hm,
              const HexAttacker& attacker,
              QVector<HexLocation> &moves);

private:

    void InitSearch(HexBoard& brd, HexColor color);

    HexPoint CheckEndgame(HexBoard& brd, HexColor color,
                 bitset_t& consider, double& score);

    bool PerformPreSearch(HexBoard& brd, HexColor color,
                          bitset_t& consider, PointSequence& winningSequence);


    HexEnvironment* en;
    StoneBoard* brd;
    QElapsedTimer* usedTime;
    const uint& runTime;
};

#endif // MOWORK_H
