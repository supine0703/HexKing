#ifndef GAMEPVE_H
#define GAMEPVE_H

#include "GameMode.h"

#include <QThread>

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

    void AIWork() override;
    bool IsPlayer() override;
    void Determine(HexAttacker _attacker) override;

private:
    HexAttacker thisAttacker;
    HexAttacker *nowAttacker;
};

#endif // GAMEPVE_H
