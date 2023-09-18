#include "GamePvP.h"

GamePvP::GamePvP(bool *end,
    HexMatch *_match,
    QVector<HexPoint> *_winner,
    HexAttacker *_attacker,
    QObject *parent)
    : GameMode::GameMode(end, _match, _winner, _attacker, parent)
{
}

bool GamePvP::IsPlayer()
{
    return true;
}
