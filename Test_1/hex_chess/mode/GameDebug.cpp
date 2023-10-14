#include "GameDebug.h"

#include <HexLog.h>
#include <QStack>

GameDebug::GameDebug(
    bool *end,
    HexBoard *_board,
    QVector<HexPoint> *_winner,
    HexAttacker *_attacker, QObject *parent)
    : GameMode(end, _board, _winner, _attacker, parent)
    , nowAttacker(_attacker)
    , history(new QStack<HexPoint>)
{
    choose_ai(3, blackAI, whiteAI);
}

GameDebug::~GameDebug()
{
    delete blackAI;
    delete whiteAI;
    delete history;
}

void GameDebug::AIWork()
{
    if (*(*nowAttacker))
    {
        hexLog << dm << whiteAI->Name() << hst::whtai;
        auto [row, col] = whiteAI->ChooseMove(*board, *nowAttacker);
        hexLog << whiteAI->Name() << "placed" << "white"
               << "(" << row << "," << col << ")" << hlg::wdl << hlg::ln;
        emit placeChess(row, col);
    }
    else
    {
        hexLog << dm << blackAI->Name() << hst::blkai;
        auto [row, col] = blackAI->ChooseMove(*board, *nowAttacker);
        hexLog << blackAI->Name() << "placed" << "black"
               << "(" << row << "," << col << ")" << hlg::bdl << hlg::ln;
        emit placeChess(row, col);
    }
}

bool GameDebug::IsPlayer()
{
    return false;
}

void GameDebug::RegretAMove()
{
    HexPoint move = history->pop();

    Q_ASSERT((*board)(move) != HexCell::Empty);
    *nowAttacker = !(*nowAttacker);
    hexLog << "Regret a move" << (*(*nowAttacker) ? "White" : "Black")
           <<"(" << move.row << "," << move.col << ")"
           << (*nowAttacker == HexAttacker::Black ? hlg::bdl : hlg::wdl);
    (*board)(move, HexCell::Empty);

    board->subPiece();
    board->subPiece();
    emit placeChess(-1, -1);
}

void GameDebug::AddHistory(int row, int col)
{
    history->push({row, col});
}

void GameDebug::StopAI()
{
    whiteAI->StopWork();
    blackAI->StopWork();
}
