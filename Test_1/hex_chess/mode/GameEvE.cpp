#include "GameEvE.h"

#include <QDebug>

GameEvE::GameEvE(
    bool *end,
    HexBoard *_board,
    QVector<HexPoint> *_winner,
    HexAttacker *_attacker,
    QObject *parent)
    : GameMode(end, _board, _winner, _attacker, parent)
    , nowAttacker(_attacker)
{
    choose_ai(3, blackAI, whiteAI);
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
        auto [row, col] = whiteAI->ChooseMove(*board, *nowAttacker);
        emit placeChess(row, col);
    }
    else
    {
        qDebug() << "----------Black: " + blackAI->Name() + " is thinking----------" << Qt::endl;
        auto [row, col] = blackAI->ChooseMove(*board, *nowAttacker);
        emit placeChess(row, col);
    }
}

bool GameEvE::IsPlayer()
{
    return false;
}

