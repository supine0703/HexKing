#ifndef GAMEDEBUG_H
#define GAMEDEBUG_H

#include "GameMode.h"

#include "HexAI.hpp"

class GameDebug : public GameMode, private ChooseAI
{

public:
    GameDebug(
        bool *end,
        HexBoard *_board,
        QVector<HexPoint> *_winner,
        HexAttacker *_attacker,
        QObject *parent = nullptr
    );
    ~GameDebug();

    void AIWork() override;
    bool IsPlayer() override;
    void RegretAMove();
    void AddHistory(int row, int col);

private:
    HexAttacker *nowAttacker;
    HexAI* blackAI;
    HexAI* whiteAI;

    QStack<HexPoint>* history;

};

#endif // GAMEDEBUG_H
