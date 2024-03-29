#include "AI_Mcts_G.h"

#include <QThreadPool>
#include <QElapsedTimer>
#include "MctsWork_G.h"

#include "HexLog.h"

AI_Mcts_G::AI_Mcts_G(double ecf, uint max_decision_time, bool parallelized)
    : HexAI()
    , ecf(ecf)
    , endTime(max_decision_time * 1000)
    , copyTime(endTime)
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

HexLocation AI_Mcts_G::ChooseMove(const HexMatrix &board, HexAttacker attacker)
{
    endTime = copyTime;
    if (board.PiecesNum() < 2)
    {
        int set = board.Order() / 2;
        return HexLocation(set + (board(set, set) == HexCell::Empty ? 0 : 1), set);
    }
    switch (board.PiecesNum() >> 1)
    {
    case 0:
        while (false)
        {
            int set = board.Order() / 2;
            return HexLocation(set + (board(set, set) == HexCell::Empty ? 0 : 1), set);
        }
    default:
        if (RootIteration(board))
        {
            hexLog() << "Iteration success" << "| visitsNum :" << root->VisitsNum()
                   << "(" << root->Move().row << "," << root->Move().col << ")"
                   << (attacker == HexAttacker::Black ? hlg::bdl : hlg::wdl);
            break;
        }
    case 1:
        root.reset(new MctsNode(!attacker, {-1, -1}, board.EmptyNum(), nullptr));
    }

    // Create a new root node for MCTS
    usedTime = new QElapsedTimer();
    usedTime->start();


    // Run MCTS until the timer runs out to update root's and its children's
    // statistics
    MctsSearch(board);
    // Select the child with the highest win ratio as the best move:
    int visitsTotal = root->VisitsNum();
    root = BestChild();
    root->Parent() = nullptr;
    hexLog() << root->WinsNum() << "/" << root->VisitsNum()
           << "| total:" << visitsTotal
           << "| time:" << usedTime->elapsed()
           << "| step:" << ((board.PiecesNum() + 1) >> 1)
           << (attacker == HexAttacker::Black ? hlg::bdl : hlg::wdl);
    HexLocation bestMove = root->Move();

    delete usedTime;
    usedTime = nullptr;
    return bestMove;
}

void AI_Mcts_G::StopWork()
{
    endTime = 0;
}

void AI_Mcts_G::MctsSearch(const HexMatrix &board)
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

bool AI_Mcts_G::RootIteration(const HexMatrix& board)
{
//    for (int i = 0; i < root->ExpandedNum(); i++)
//    {
//        if (board(root->Child(i)->Move()) != HexCell::Empty)
//        {
//            root = root->Child(i);
//            root->Parent() = nullptr;
//            return true;
//        }
//    }
    return false;
}
