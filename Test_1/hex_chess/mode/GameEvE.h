#ifndef GAMEEVE_H
#define GAMEEVE_H

#include "GameMode.h"

#include "HexAI.hpp"

class GameEvE : public GameMode, private ChooseAI
{
public:
    GameEvE(
        bool *end,
        HexBoard *_board,
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
