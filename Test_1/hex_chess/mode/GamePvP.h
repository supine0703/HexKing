#ifndef GAMEPVP_H
#define GAMEPVP_H

#include "GameMode.h"

class GamePvP : public GameMode
{
public:
    GamePvP(bool *end, HexMatch *_match, QVector<HexPoint> *_winner);

    bool IsPlayer() override;
    void Determine(HexAttacker _attacker) override;
};

#endif // GAMEPVP_H
