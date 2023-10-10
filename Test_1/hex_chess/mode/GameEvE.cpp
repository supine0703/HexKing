#include "GameEvE.h"

#include <QDebug>

#define _VERSION_BLACK_ 'G'
#define _VERSION_WHITE_ 'E'
#define _ECF_ 0.9
#define _TIME_ 10
#define _PARALLELIZED_ true

#if _VERSION_BLACK_ == 'A'
#include "AI_Mcts_A.h"
#define AI_BLACK new AI_Mcts_A(_ECF_, _TIME_, _PARALLELIZED_)
#elif _VERSION_BLACK_ == 'E'
#include "AI_Mcts_E.h"
#define AI_BLACK new AI_Mcts_E(_ECF_, _TIME_, _PARALLELIZED_)
#elif _VERSION_BLACK_ == 'G'
#include "AI_Mcts_G.h"
#define AI_BLACK new AI_Mcts_G(_ECF_, _TIME_, _PARALLELIZED_)
#endif

#if _VERSION_WHITE_ == 'A'
#include "AI_Mcts_A.h"
#define AI_WHITE new AI_Mcts_A(_ECF_, _TIME_, _PARALLELIZED_)
#elif _VERSION_WHITE_ == 'E'
#include "AI_Mcts_E.h"
#define AI_WHITE new AI_Mcts_E(_ECF_, _TIME_, _PARALLELIZED_)
#elif _VERSION_WHITE_ == 'G'
#include "AI_Mcts_G.h"
#define AI_WHITE new AI_Mcts_G(_ECF_, _TIME_, _PARALLELIZED_)
#endif

GameEvE::GameEvE(bool *end, HexMatch *_match,
    QVector<HexPoint> *_winner,
    HexAttacker *_attacker, QObject *parent)
    : GameMode::GameMode(end, _match, _winner, _attacker, parent)
    , nowAttacker(_attacker)
    , blackAI(AI_WHITE)
    , whiteAI(AI_BLACK)
{
}

GameEvE::~GameEvE()
{
    delete blackAI;
    delete whiteAI;
}

void GameEvE::AIWork()
{
    if (*(*nowAttacker))
    {
        qDebug() << "----------White: " + whiteAI->Name() + " is thinking----------" << Qt::endl;
        auto [row, col] = whiteAI->ChooseMove(*match, *nowAttacker);
        emit placeChess(row, col);
    }
    else
    {
        qDebug() << "----------Black: " + blackAI->Name() + " is thinking----------" << Qt::endl;
        auto [row, col] = blackAI->ChooseMove(*match, *nowAttacker);
        emit placeChess(row, col);
    }
}

bool GameEvE::IsPlayer()
{
    return false;
}

