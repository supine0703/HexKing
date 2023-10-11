#include "AI_Mcts_E.h"

#include <QEventLoop>
#include <QThreadPool>
#include <QElapsedTimer>
#include "MctsWork_E.h"

#include <QDebug>

AI_Mcts_E::AI_Mcts_E(double ecf, int max_decision_time, bool parallelized)
    : HexAI()
    , ecf(ecf)
    , endTime(max_decision_time * 1000)
    , parallelized(parallelized)
    , pool(parallelized ? new QThreadPool() : nullptr)
{
}

AI_Mcts_E::~AI_Mcts_E()
{
    if (parallelized)
    {
        exit = true;
        pool->waitForDone();
        delete pool;
    }
//    HexAI::~HexAI();
//    QObject::~QObject();
}

HexPoint AI_Mcts_E::ChooseMove(const HexBoard &board, HexAttacker attacker)
{
    // Create a new root node for MCTS
    root = QSharedPointer<MctsNode>(new MctsNode(attacker, {-1, -1}, board.EmptyNum(), nullptr));
    // Expand root based on the current game state
    ExpandNode(board);
    int mcts_itCounter = 0;
    usedTime = new QElapsedTimer();
    usedTime->start();
    // Run MCTS until the timer runs out to update root's and its children's
    // statistics
    MctsSearch(mcts_itCounter, board);
    // Select the child with the highest win ratio as the best move:
    QSharedPointer<MctsNode> bestChild = BestChild();
    qDebug() << bestChild->WinsNum() << bestChild->VisitsNum()
             << "| total:" << root->VisitsNum()
             << "| time:" << usedTime->elapsed() << Qt::endl;
    HexPoint bestMove = bestChild->Move();
    root = nullptr;
    delete usedTime;
    usedTime = nullptr;
    return bestMove;
}

void AI_Mcts_E::ExpandNode(const HexBoard &board)
{
    QVector<HexPoint> validMoves = GetValidMoves_E(board);
    for (const auto& move : validMoves)
    {
        root->Expand(new MctsNode(root->Attacker(), move, board.EmptyNum(), root));
    }
}

void AI_Mcts_E::MctsSearch(int &itCounter, const HexBoard &board)
{
    if (parallelized)
    {
        /**
         * @brief boardCopy, a copy of board.
         * @bug I don't know why the cells (QVector member of MctsWork) will overflow.
         * This happens if the board (local variable) is not copied.
         * @resolve I guess the problem may come from QVector beacuse the bug didn't
         * reappear after I replaced QVector with a self-defined array.
         */
//        HexMatch boardCopy(board);
        while (!exit && usedTime->elapsed() < endTime)
        {
//            MctsWork *work = new MctsWork(SelectChildPlayout(), boardCopy);
            MctsWork_E *work = new MctsWork_E(SelectChildPlayout(), board);
            if (!pool->tryStart(work))
            {
                delete work;
            }
        }
        pool->waitForDone();
    }
    else
    {
        while (usedTime->elapsed() < endTime)
        {
            QSharedPointer<MctsNode> chosenChild = SelectChildPlayout();
            MctsWork_E(chosenChild, board).run();
            itCounter++;
        }
    }
}

QSharedPointer<MctsNode> AI_Mcts_E::SelectChildPlayout()
{
    //Initialize best_child as the first child and calculate its UCT score
    QSharedPointer<MctsNode> bestChild = root->Child(0);
    double maxScore = UCTScore(bestChild);
    // Iterate over the remaining child nodes to find the one with the highest UCT score
    for (int i = 1, end = root->ExpandedNum(); i < end; i++)
    {
        const auto& child = root->Child(i);
        double uctScore = UCTScore(child);
        if (uctScore > maxScore)
        {
            maxScore = uctScore;
            bestChild = child;
        }
    }
    return bestChild;
}

double AI_Mcts_E::UCTScore(const QSharedPointer<MctsNode> &child)
{
    // If any child node has not been visited yet, return a high value to
    // encourage exploration
    if (child->VisitsNum() == 0)
    {
        return qInf(); // double max
    }
    else
    {
        // Otherwise, calculate the UCT score using the UCT formula.
        return static_cast<double>(child->WinsNum()) / child->VisitsNum()
            + ecf * qSqrt(qLn(static_cast<double>(root->VisitsNum())) / child->VisitsNum()
        );
    }
}

QSharedPointer<MctsNode> AI_Mcts_E::BestChild()
{
    double maxWinRatio = -1;
    QSharedPointer<MctsNode> bestChild;
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
    Q_ASSERT(bestChild);
    return bestChild;
}
