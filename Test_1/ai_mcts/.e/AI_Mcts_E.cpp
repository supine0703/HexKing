#include "AI_Mcts_E.h"

#include <QEventLoop>
#include <QThreadPool>
#include <QElapsedTimer>
#include "MctsWork.h"

#include <QDebug>

AI_Mcts_E::AI_Mcts_E(double ecf, int max_decision_time, bool parallelized, QObject *parent)
    : QObject(parent)
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
        pool->waitForDone();
        delete pool;
    }
}

HexPoint AI_Mcts_E::ChooseMove(const HexMatch &board, HexAttacker attacker)
{
    // Create a new root node for MCTS
    root = QSharedPointer<MctsNode>(new MctsNode(attacker, {-1, -1}, nullptr));
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
    qDebug() << bestChild->GetWinsNum() << bestChild->GetVisitedNum()
             << "| total:" << root->GetVisitedNum() << Qt::endl;
    HexPoint bestMove = bestChild->GetMove();
    root = nullptr;
    delete usedTime;
    usedTime = nullptr;
    return bestMove;
}

void AI_Mcts_E::ExpandNode(const HexMatch &board)
{
    QVector<HexPoint> validMoves = GetValidMoves(board);
    for (const auto& move : validMoves)
    {
        root->children.push_back(
            QSharedPointer<MctsNode>(new MctsNode(root->GetAttacker(), move, root))
        );
    }
}

void AI_Mcts_E::MctsSearch(int &itCounter, const HexMatch &board)
{
    if (parallelized)
    {
        /**
         * @brief boardCopy, a copy of board.
         * @bug I don't know why the cells (QVector member of MctsWork) will overload.
         * This happens if the board (local variable) is not copied.
         */
        HexMatch boardCopy(board);
        while (usedTime->elapsed() < endTime)
        {
            MctsWork *work = new MctsWork(SelectChildPlayout(), boardCopy);
            if (!pool->tryStart(work))
            {
                work->deleteLater();
            }
        }
        pool->waitForDone();
    }
    else
    {
        while (usedTime->elapsed() < endTime)
        {
            QSharedPointer<MctsNode> chosenChild = SelectChildPlayout();
            MctsWork(chosenChild, board).run();
            itCounter++;
        }
    }
}

QSharedPointer<MctsNode> AI_Mcts_E::SelectChildPlayout()
{
    //Initialize best_child as the first child and calculate its UCT score
    QSharedPointer<MctsNode> bestChild = root->children[0];
    double maxScore = UCTScore(bestChild);
    // Iterate over the remaining child nodes to find the one with the highest UCT score
    for (int i = 1, end = root->children.count(); i < end; i++)
    {
        const auto& child = root->children[i];
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
    if (child->GetVisitedNum() == 0)
    {
        return qInf(); // double max
    }
    else
    {
        // Otherwise, calculate the UCT score using the UCT formula.
        return static_cast<double>(child->GetWinsNum()) / child->GetVisitedNum()
            + ecf * qSqrt(qLn(root->GetVisitedNum()) / child->GetVisitedNum()
        );
    }
}

QSharedPointer<MctsNode> AI_Mcts_E::BestChild()
{
    double maxWinRatio = -1;
    QSharedPointer<MctsNode> bestChild;
    for (const auto& child : root->children)
    {
        double winRatio = static_cast<double>(child->GetWinsNum()) / child->GetVisitedNum();
        if (winRatio > maxWinRatio)
        {
            maxWinRatio = winRatio;
            bestChild = child;
        }
    }
    Q_ASSERT(bestChild);
    return bestChild;
}
