#include "GameDebug.h"

#include "QDebug"
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

bool GameDebug::IsPlayer()
{
    return false;
}

void GameDebug::RegretAMove()
{
    HexPoint move = history->pop();

    Q_ASSERT((*board)(move) != HexCell::Empty);
    *nowAttacker = !(*nowAttacker);
    qDebug() << "Regret a move" << (*(*nowAttacker) ? "White" : "Black")
             <<"(" << move.row << "," << move.col << ")" << Qt::endl;
    (*board)(move, HexCell::Empty);

    board->subPiece();
    board->subPiece();
    emit placeChess(-1, -1);
}

void GameDebug::AddHistory(int row, int col)
{
    history->push({row, col});
}
