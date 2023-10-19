#include "AI_Mcts_I.h"

#include <QThreadPool>
#include <QElapsedTimer>
#include "MctsWork_I.h"

#include "MoWork.h"
#include "HexLog.h"
#include "qrandom.h"

AI_Mcts_I::AI_Mcts_I(double ecf, uint max_decision_time, bool parallelized)
    : HexAI()
    , ecf(ecf)
    , endTime(max_decision_time * 1000)
    , copyTime(endTime)
    , parallelized(parallelized)
    , pool(parallelized ? new QThreadPool() : nullptr)
    , mow(new MoWork(endTime))
{
}

AI_Mcts_I::~AI_Mcts_I()
{
    if (parallelized)
    {
        endTime = 0;
        pool->waitForDone();
        delete pool;
    }
}

HexLocation AI_Mcts_I::ChooseMove(const HexMatrix &board, HexAttacker attacker)
{
    endTime = copyTime;
    if (board.PiecesNum() < 2)
    {
        int set = board.Order() / 2;
        return HexLocation(set + (board(set, set) == HexCell::Empty ? 0 : 1), set);
    }

    // Create a new root node for MCTS
    usedTime = new QElapsedTimer();
    usedTime->start();

//    switch (board.PiecesNum() >> 1)
//    {
//    case 0:
//        while (false)
//        {
//            int set = board.Order() / 2;
//            return HexLocation(set + (board(set, set) == HexCell::Empty ? 0 : 1), set);
//        }
//    default:
//        if (RootIteration(board))
//        {
//            hexLog() << "Iteration success" << "| visitsNum :" << root->VisitsNum()
//                   << "(" << root->Move().row << "," << root->Move().col << ")"
//                   << (attacker == HexAttacker::Black ? hlg::bdl : hlg::wdl);
//            break;
//        }
//    case 1:
//        root.reset(new MctsNode(!attacker, {-1, -1}, board.EmptyNum(), nullptr));
//    }

    QVector<HexLocation> moves;
    mow->Work(usedTime, board, attacker, moves);
    if (moves.count() == 1)
    {
        delete usedTime;
        usedTime = nullptr;
        return moves[0];
    }

    hexLog() << "VCs used time:" << usedTime->elapsed()
             << (attacker == HexAttacker::Black ? hlg::bdl : hlg::wdl);

    if (usedTime->elapsed() > endTime)
    {
        QVector<HexLocation> validMoves = GetValidMoves_I(board);
        static QRandomGenerator random;
        return validMoves[random.bounded(validMoves.count())];
    }

    root.reset(new MctsNode_I(!attacker, {-1, -1}, moves.count(), nullptr));
    root->Set(moves);

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

void AI_Mcts_I::StopWork()
{
    endTime = 0;
}

void AI_Mcts_I::MctsSearch(const HexMatrix &board)
{
    if (parallelized)
    {
        MctsWork_I *work;
        while(
            pool->tryStart(
                work = new MctsWork_I(root, board, *usedTime, endTime, ecf, *mow)
            )
        );
        delete work;
        work = nullptr;
        pool->waitForDone();
    }
    else
    {
        MctsWork_I(root, board, *usedTime, endTime, ecf, *mow).run();
    }
}

QSharedPointer<MctsNode_I> AI_Mcts_I::BestChild()
{
    QSharedPointer<MctsNode_I> bestChild;

    double maxWinRatio = 0;
    for (int i = 0; i < 121; i++)
    {
        if (!root->Allow(i)) continue;
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

bool AI_Mcts_I::RootIteration(const HexMatrix& board)
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
