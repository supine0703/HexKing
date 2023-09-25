#include "GamePvP.h"

GamePvP::GamePvP(bool *end,
    HexBoard *_board,
    QVector<HexPoint> *_winner,
    HexAttacker *_attacker,
    QObject *parent)
    : GameMode::GameMode(end, _board, _winner, _attacker, parent)
{
}

bool GamePvP::IsPlayer()
{
    return true;
}
