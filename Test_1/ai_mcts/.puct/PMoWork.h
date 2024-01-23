#ifndef PMOWORK_H
#define PMOWORK_H

#include "HexMatrix.hpp"

class StoneBoard;
class QElapsedTimer;
struct HexEnvironment;
class HexBoard;

#include "HexColor.hpp"
#include "HexPoint.hpp"

HEXPOINT_HPP

class PMoWork
{
public:
    PMoWork(const uint& runTime);
    ~PMoWork();

    HexLocation Work(QElapsedTimer* time, const HexMatrix& hm,
              const HexAttacker& attacker);

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

#endif // PMOWORK_H
