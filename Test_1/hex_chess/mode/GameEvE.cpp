#include "GameEvE.h"

#include "HexLog.h"

GameEvE::GameEvE(
    bool *end,
    HexMatrix *_board,
    QVector<HexLocation> *_winner,
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
        hexLog() << whiteAI->Name() << hst::whtai;
        auto [row, col] = whiteAI->ChooseMove(*board, *nowAttacker);
        hexLog() << whiteAI->Name() << "placed" << "white"
               << "(" << row << "," << col << ")" << hlg::wdl << hlg::ln;
        emit placeChess(row, col);
    }
    else
    {
        hexLog() << blackAI->Name() << hst::blkai;
        auto [row, col] = blackAI->ChooseMove(*board, *nowAttacker);
        hexLog() << blackAI->Name() << "placed" << "black"
               << "(" << row << "," << col << ")" << hlg::bdl << hlg::ln;
        emit placeChess(row, col);
    }
}

bool GameEvE::IsPlayer()
{
    return false;
}

