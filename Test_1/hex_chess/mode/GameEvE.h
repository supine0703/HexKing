#ifndef GAMEEVE_H
#define GAMEEVE_H

#include "GameMode.h"

class HexAI;

class GameEvE : public GameMode
{
public:
    GameEvE(
        bool *end,
        HexMatch *_match,
        QVector<HexPoint> *_winner,
        HexAttacker *_attacker,
        QObject *parent = nullptr
    );
    ~GameEvE();

    void AIWork() override;
    bool IsPlayer() override;

private:
    HexAttacker *nowAttacker;
    HexAI *blackAI;
    HexAI *whiteAI;
};

#endif // GAMEEVE_H
