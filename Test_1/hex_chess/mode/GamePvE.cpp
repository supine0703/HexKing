#include "GamePvE.h"

#include <QDebug>

GamePvE::GamePvE(
    bool *end,
    HexBoard *_board,
    QVector<HexPoint> *_winner,
    HexAttacker *_attacker,
    bool isWhite,
    QObject *parent)
    : GameMode(end, _board, _winner, _attacker, parent)
    , thisAttacker(static_cast<HexAttacker>(isWhite))
{
    choose_ai(1 + isWhite, AI, AI);
}

GamePvE::~GamePvE()
{
    delete AI;
}

void GamePvE::AIWork()
{
    qDebug() << "----------" + AI->Name() + " is thinking----------" << Qt::endl;
    auto [row, col] = AI->ChooseMove(*board, *nowAttacker);
    emit placeChess(row, col);
}

bool GamePvE::IsPlayer()
{
    return *nowAttacker != thisAttacker;
}

