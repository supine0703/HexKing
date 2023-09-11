#include "GamePvP.h"

GamePvP::GamePvP(bool *end, HexMatch *_match, QVector<HexPoint> *_winner, QObject *parent)
    : GameMode::GameMode(end, _match, _winner, parent)
{
}

bool GamePvP::IsPlayer()
{
    return true;
}

void GamePvP::Determine(HexAttacker _attacker)
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
