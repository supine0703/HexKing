#include "GamePvE.h"

#include <QDebug>

#define _VERSION_ 'E'
#define _ECF_ 1.41
#define _TIME_ 5
#define _PARALLELIZED_ true

#if _VERSION_ == 'A'
#include "AI_Mcts_A.h"
#elif _VERSION_ == 'E'
#include "AI_Mcts_E.h"
#endif

GamePvE::GamePvE(
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

void GamePvE::AIWork()
{
#if _VERSION_ == 'A'
    qDebug() << "----------AI_Mcts_A is thinking----------" << Qt::endl;
    AI_Mcts_A AI(_ECF_, _TIME_, _PARALLELIZED_, this);
#elif _VERSION_ == 'E'
    qDebug() << "----------AI_Mcts_E is thinking----------" << Qt::endl;
    AI_Mcts_E AI(_ECF_, _TIME_, _PARALLELIZED_, this);
#endif
    auto [row, col] = AI.ChooseMove(*match, *nowAttacker);
    emit placeChess(row, col);
}

bool GamePvE::IsPlayer()
{
    return *nowAttacker != thisAttacker;
}

void GamePvE::Determine(HexAttacker _attacker)
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
