#include "GamePvE.h"

#include "HexLog.h"

GamePvE::GamePvE(
    bool *end,
    HexMatrix *_board,
    QVector<HexLocation> *_winner,
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
    hexLog() << AI->Name() << (*(*nowAttacker) ? hst::whtai : hst::blkai);
    auto [row, col] = AI->ChooseMove(*board, *nowAttacker);
    hexLog() << AI->Name() << "placed" << (*(*nowAttacker) ? "white" : "black")
           << "(" << row << "," << col << ")"
           << (*(*nowAttacker) ? hlg::wdl : hlg::bdl) << hlg::ln;
    emit placeChess(row, col);
}

bool GamePvE::IsPlayer()
{
    return *nowAttacker != thisAttacker;
}

