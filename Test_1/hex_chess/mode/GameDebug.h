#ifndef GAMEDEBUG_H
#define GAMEDEBUG_H

#include "GameMode.h"

#include "HexAI.hpp"

class GameDebug : public GameMode, private ChooseAI
{
public:
    GameDebug(
        bool *end,
        HexMatrix *_board,
        QVector<HexLocation> *_winner,
        HexAttacker *_attacker,
        QObject *parent = nullptr
    );
    ~GameDebug();

    void AIWork() override;
    bool IsPlayer() override;
    void RegretAMove();
    void AddHistory(int row, int col);
    void StopAI();
    HexLocation GetRegret(int step);

    QString dm = "";

private:
    HexAttacker *nowAttacker;
    HexAI* blackAI;
    HexAI* whiteAI;
    
    QStack<HexLocation>* history;

public:
    void Exit() override;
    void Init();
};

#endif // GAMEDEBUG_H
