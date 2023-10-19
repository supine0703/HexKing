#ifndef GAMEPVE_H
#define GAMEPVE_H

#include "GameMode.h"

#include "HexAI.hpp"

class GamePvE : public GameMode, private ChooseAI
{
public:
    GamePvE(
        bool *end,
        HexMatrix *_board,
        QVector<HexLocation> *_winner,
        HexAttacker *_attacker,
        bool isWhite,
        QObject *parent = nullptr
    );
    ~GamePvE() override;

    void AIWork() override;
    bool IsPlayer() override;

private:
    HexAttacker thisAttacker;
    HexAI* AI;
};

#endif // GAMEPVE_H
