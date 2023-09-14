#include "GameEvE.h"

#include <QDebug>

//可修改版本
#define _VERSION_1 'E'
#define _VERSION_2 'A'

#define _ECF_ 1.41
#define _TIME_ 5
#define _PARALLELIZED_ true

#if _VERSION_1 == 'A'
#include "AI_Mcts_A.h"
#elif _VERSION_1 == 'E'
#include "AI_Mcts_E.h"
#endif

#if _VERSION_2 == 'A'
#include "AI_Mcts_A.h"
#elif _VERSION_2 == 'E'
#include "AI_Mcts_E.h"
#endif

GameEvE::GameEvE(
    bool *end,
    HexMatch *_match,
    QVector<HexPoint> *_winner,
    HexAttacker *_attacker,
    bool isWhite,
    QObject *parent)
    : GameMode::GameMode(end, _match, _winner, parent)
    , nowAttacker(_attacker)
    , thisAttacker(static_cast<HexAttacker>(isWhite))
{
}

void GameEvE::AIWork1()
{
#if _VERSION_1 == 'A'
    qDebug() << "----------AI_Mcts_A is thinking----------" << Qt::endl;
    AI_Mcts_A *AI(new AI_Mcts_A(_ECF_, _TIME_, _PARALLELIZED_));
#elif _VERSION_1 == 'E'
    qDebug() << "----------AI_Mcts_E is thinking----------" << Qt::endl;
    AI_Mcts_E *AI1(new AI_Mcts_E(_ECF_, _TIME_, _PARALLELIZED_, this));
#endif
    auto [row, col] = AI1->ChooseMove(*match, *nowAttacker);
    emit placeChess(row, col);
}

void GameEvE::AIWork2()
{
#if _VERSION_2 == 'A'
    qDebug() << "----------AI_Mcts_A is thinking----------" << Qt::endl;
    AI_Mcts_A *AI2(new AI_Mcts_A(_ECF_, _TIME_, _PARALLELIZED_));
#elif _VERSION_2 == 'E'
    qDebug() << "----------AI_Mcts_E is thinking----------" << Qt::endl;
    AI_Mcts_E *AI(new AI_Mcts_E(_ECF_, _TIME_, _PARALLELIZED_, this));
#endif
    auto [row, col] = AI2->ChooseMove(*match, *nowAttacker);
    emit placeChess(row, col);
}

bool GameEvE::IsPlayer()
{
    return false;
}


void GameEvE::Determine(HexAttacker _attacker)
{
    if (*end)
    {
        return;
    }

    attacker = _attacker;
    if (Outcome())
    {
        *end = true;
        return;
    }
}
