#include "GamePvP.h"

GamePvP::GamePvP(bool *end,
                 HexMatrix *_board,
                 QVector<HexLocation> *_winner,
    HexAttacker *_attacker,
    QObject *parent)
    : GameMode::GameMode(end, _board, _winner, _attacker, parent)
{
}

bool GamePvP::IsPlayer()
{
    return true;
}
