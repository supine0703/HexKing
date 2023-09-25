#include "AI_Mcts_G.h"

#include <QThreadPool>
#include <QElapsedTimer>
#include "MctsWork_G.h"

#include <QDebug>

AI_Mcts_G::AI_Mcts_G(double ecf, uint max_decision_time, bool parallelized)
    : HexAI()
    , ecf(ecf)
    , endTime(max_decision_time * 1000)
    , parallelized(parallelized)
    , pool(parallelized ? new QThreadPool() : nullptr)
{
}

AI_Mcts_G::~AI_Mcts_G()
{
    if (parallelized)
    {
        endTime = 0;
        pool->waitForDone();
        delete pool;
    }
}

HexPoint AI_Mcts_G::ChooseMove(const HexBoard &board, HexAttacker attacker)
{
    if (board.PiecesNum() < 2)
    {
        int set = board.Order() / 2;
        return HexPoint(set + (board(set, set) == HexCell::Empty ? 0 : 1), set);
    }
    // Create a new root node for MCTS
    usedTime = new QElapsedTimer();
    usedTime->start();
    root.reset(new MctsNode(attacker, {-1, -1}, board.EmptyNum(), nullptr));

    // Run MCTS until the timer runs out to update root's and its children's
    // statistics
    MctsSearch(board);
    // Select the child with the highest win ratio as the best move:
    QSharedPointer<MctsNode> bestChild = BestChild();
    qDebug() << bestChild->WinsNum() << bestChild->VisitsNum()
             << "| total:" << root->VisitsNum()
             << "| time:" << usedTime->elapsed()
             << "| step:" << ((board.PiecesNum() + 1) >> 1)
             << Qt::endl;
    HexPoint bestMove = bestChild->Move();

    root.clear();
    delete usedTime;
    usedTime = nullptr;

    return bestMove;
}

void AI_Mcts_G::MctsSearch(const HexBoard &board)
{
    if (parallelized)
    {
        /**
         * @brief boardCopy, a copy of board.
         * @bug I don't know why the cells (QVector member of MctsWork) will overflow.
         * This happens if the board (local variable) is not copied.
         * @resolve I guess the problem may come from QVector beacuse the bug didn't
         * reappear after I replaced QVector with a self-defined array. --2023.9.17
         */
        MctsWork_G *work;
        while(pool->tryStart(work = new MctsWork_G(root, board, *usedTime, endTime, ecf)));
        delete work;
        work = nullptr;
        pool->waitForDone();
    }
    else
    {
        MctsWork_G(root, board, *usedTime, endTime, ecf).run();
    }
}

QSharedPointer<MctsNode> AI_Mcts_G::BestChild()
{
    QSharedPointer<MctsNode> bestChild;

    double maxWinRatio = 0;
    for (int i = 0; i < root->ExpandedNum(); i++)
    {
        double winRatio =
            static_cast<double>(root->Child(i)->WinsNum()) / root->Child(i)->VisitsNum();
        if (winRatio > maxWinRatio)
        {
            maxWinRatio = winRatio;
            bestChild = root->Child(i);
        }
    }

//    int maxVisit = 0;
//    for (int i = 0; i < root->ExpandedNum(); i++)
//    {
//        int visits = root->Child(i)->VisitsNum();
//        if (visits > maxVisit)
//        {
//            maxVisit = visits;
//            bestChild = root->Child(i);
//        }
//    }
    Q_ASSERT(bestChild);
    return bestChild;
}
