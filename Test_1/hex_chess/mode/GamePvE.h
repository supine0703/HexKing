#ifndef GAMEPVE_H
#define GAMEPVE_H

#include "GameMode.h"

class HexAI;

class GamePvE : public GameMode
{
public:
    GamePvE(
        bool *end,
        HexMatch *_match,
        QVector<HexPoint> *_winner,
        HexAttacker *_attacker,
        bool isWhite,
        QObject *parent = nullptr
    );
    ~GamePvE() override;

    void AIWork() override;
    bool IsPlayer() override;

private:
    HexAttacker thisAttacker;
    HexAI *AI;
};

#endif // GAMEPVE_H
