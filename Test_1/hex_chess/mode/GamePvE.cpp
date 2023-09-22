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
#elif _VERSION_ == 'G'
#include "AI_Mcts_G.h"
#endif

GamePvE::GamePvE(
    bool *end,
    HexMatch *_match,
    QVector<HexPoint> *_winner,
    HexAttacker *_attacker,
    bool isWhite,
    QObject *parent)
    : GameMode::GameMode(end, _match, _winner, _attacker, parent)
    , thisAttacker(static_cast<HexAttacker>(isWhite))
#if _VERSION_ == 'A'
    , AI(new AI_Mcts_A(_ECF_, _TIME_, _PARALLELIZED_))
#elif _VERSION_ == 'E'
    , AI(new AI_Mcts_E(_ECF_, _TIME_, _PARALLELIZED_))
#elif _VERSION_ == 'G'
    , AI(new AI_Mcts_G(_ECF_, _TIME_, _PARALLELIZED_))
#endif
{
}

GamePvE::~GamePvE()
{
    delete AI;
}

void GamePvE::AIWork()
{
    qDebug() << "----------" + AI->Name() + " is thinking----------" << Qt::endl;
    auto [row, col] = AI->ChooseMove(*match, *nowAttacker);
    emit placeChess(row, col);
}

bool GamePvE::IsPlayer()
{
    return *nowAttacker != thisAttacker;
}

