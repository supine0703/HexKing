#include "GameEvE.h"

#include <QDebug>

#define _VERSION_BLACK_ 'A'
#define _VERSION_WHITE_ 'E'
#define _ECF_ 1.41
#define _TIME_ 5
#define _PARALLELIZED_ true

#if _VERSION_BLACK_ == 'A' || _VERSION_WHITE_ == 'A'
#include "AI_Mcts_A.h"
#endif
#if _VERSION_BLACK_ == 'E' || _VERSION_WHITE_ == 'E'
#include "AI_Mcts_E.h"
#endif

GameEvE::GameEvE(bool *end, HexMatch *_match,
    QVector<HexPoint> *_winner,
    HexAttacker *_attacker, QObject *parent)
    : GameMode::GameMode(end, _match, _winner, _attacker, parent)
    , nowAttacker(_attacker)
#if _VERSION_BLACK_ == 'A'
    , blackAI(new AI_Mcts_A(_ECF_, _TIME_, _PARALLELIZED_))
#elif _VERSION_BLACK_ == 'E'
    , blackAI(new AI_Mcts_E(_ECF_, _TIME_, _PARALLELIZED_))
#endif
#if _VERSION_WHITE_ == 'A'
    , whiteAI(new AI_Mcts_A(_ECF_, _TIME_, _PARALLELIZED_))
#elif _VERSION_WHITE_ == 'E'
    , whiteAI(new AI_Mcts_E(_ECF_, _TIME_, _PARALLELIZED_))
#endif
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
        qDebug() << "----------White: " + whiteAI->GetName() + " is thinking----------" << Qt::endl;
        auto [row, col] = whiteAI->ChooseMove(*match, *nowAttacker);
        emit placeChess(row, col);
    }
    else
    {
        qDebug() << "----------Black: " + blackAI->GetName() + " is thinking----------" << Qt::endl;
        auto [row, col] = blackAI->ChooseMove(*match, *nowAttacker);
        emit placeChess(row, col);
    }
}

bool GameEvE::IsPlayer()
{
    return false;
}

