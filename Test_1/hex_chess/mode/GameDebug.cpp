#include "GameDebug.h"

#include "HexLog.h"
#include <QStack>
#include "ShareData.h"

GameDebug::GameDebug(
    bool *end,
    HexMatrix *_board,
    QVector<HexLocation> *_winner,
    HexAttacker *_attacker, QObject *parent)
    : GameMode(end, _board, _winner, _attacker, parent)
    , nowAttacker(_attacker)
    , history(new QStack<HexLocation>)
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
        hexLog() << dm << whiteAI->Name() << hst::whtai;
        auto move = whiteAI->ChooseMove(*board, *nowAttacker);
        hexLog() << whiteAI->Name() << "placed" << "white"
                 << move.Str() << hlg::wdl << hlg::ln;
        emit placeChess(move.row, move.col);
    }
    else
    {
        hexLog() << dm << blackAI->Name() << hst::blkai;
        auto move = blackAI->ChooseMove(*board, *nowAttacker);
        hexLog() << blackAI->Name() << "placed" << "black"
                 << move.Str() << hlg::bdl << hlg::ln;
        emit placeChess(move.row, move.col);
    }
}

bool GameDebug::IsPlayer()
{
    return false;
}

void GameDebug::RegretAMove()
{
    HexLocation move = history->pop();
    hisMove().pop_back();

    Q_ASSERT((*board)(move) != HexCell::Empty);
    hexLog() << "Regret a move" << (*(*nowAttacker) ? "White" : "Black")
           << move.Str()
           << (*nowAttacker == HexAttacker::Black ? hlg::bdl : hlg::wdl);
    (*board)(move, HexCell::Empty);

    board->subPiece();
    board->subPiece();
    emit placeChess(-1, -1);
}

void GameDebug::AddHistory(int row, int col)
{
    history->push({row, col});
    hisMove().push_back(row * 11 + col);
}

void GameDebug::StopAI()
{
    whiteAI->StopWork();
    blackAI->StopWork();
}

HexLocation GameDebug::GetRegret(int step)
{
    if (history->size() < step)
    {
        return {-1, -1};
    }
    else
    {
        return (*history)[step-1];
    }
}

void GameDebug::Exit()
{
    blackAI->Exit();
    whiteAI->Exit();
    rdy() = true;
}

void GameDebug::Init()
{
    blackAI->Init();
    whiteAI->Init();
    emit placeChess(250, 0);
}
