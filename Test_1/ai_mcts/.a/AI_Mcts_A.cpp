#include "AI_Mcts_A.h"

#include <QThread>
#include <QStack>
#include <QtMath>
#include <thread>
#include "MctsNode.hpp"

AI_Mcts_A::AI_Mcts_A(double ecf, qint64 max_decision_time, bool parallelized)
    : HexAI()
    , ecf(ecf)
    , usedTime()
    , endTime(max_decision_time * 1000)
    , parallelized(parallelized)
{
}

HexPoint AI_Mcts_A::ChooseMove(const HexMatch &board, HexAttacker attacker)
{
    // Create a new root node for MCTS
    root = QSharedPointer<MctsNode>(new MctsNode(attacker, qMakePair(-1, -1), nullptr));
    // Prepare for potential parallelism
    uint number_of_threads = 1;
    if (parallelized) {
        // Determine the maximum number of threads available on the hardware.
        number_of_threads = std::thread::hardware_concurrency();
    }
    // Expand root based on the current game state
    ExpandNode(root, board);
    int mcts_itCounter = 0;
    usedTime.start();
    // Run MCTS until the timer runs out to update root's and its children's
    // statistics
    MtcsSearch(mcts_itCounter, board, number_of_threads);
    // Select the child with the highest win ratio as the best move:
    QSharedPointer<MctsNode> bestChild = BestChild();
    qDebug() << bestChild->GetWinsNum() << bestChild->GetVisitedNum()
             << "| total:" << root->GetVisitedNum()
             << "| time:" << usedTime.elapsed() << Qt::endl;
    return bestChild->GetMove();
}

void AI_Mcts_A::ExpandNode(const QSharedPointer<MctsNode> &node, const HexMatch &board)
{
    QVector<HexPoint> validMoves = GetValidMoves(board);
    for (const auto& move : validMoves)
    {
        node->children.push_back(
            QSharedPointer<MctsNode>(new MctsNode(node->GetAttacker(), move, node))
        );
    }
}

void AI_Mcts_A::MtcsSearch(
    int &itCounter,
    const HexMatch &board,
    uint number_of_threads)
{
    while (!exit && usedTime.elapsed() < endTime)
    {
        // Select a child node for playout using UCT
        QSharedPointer<MctsNode> chosenChild = SelectChildPlayout(root);
        // If parallelization is enabled, run playouts concurrently:
        if (parallelized)
        {
            QVector<HexAttacker> results = ParallelPlayout(
                chosenChild, board, number_of_threads);
            // Backpropagate each of the results
            for (HexAttacker playoutWinner : results) {
                Backpropagate(chosenChild, playoutWinner);
            }

        }
        else // Else, just do a single playout:
        {
            HexAttacker playout_winner = SimulatePlayout(chosenChild, board);
            Backpropagate(chosenChild, playout_winner);
        }
        // Print statistics:
        itCounter++;
    }
}

QSharedPointer<MctsNode> AI_Mcts_A::SelectChildPlayout(
    const QSharedPointer<MctsNode> &parent)
{
    // Initialize best_child as the first child and calculate its UCT score
    QSharedPointer<MctsNode> best_child = parent->children[0];
    double max_score = UCTScore(best_child, parent);
    // Iterate over the remaining child nodes to find the one with the highest UCT score
    for (int i = 1, end = parent->children.count(); i < end; i++)
    {
        const auto& child = parent->children[i];
        double uct_score = UCTScore(child, parent);

        if (uct_score > max_score)
        {
            max_score = uct_score;
            best_child = child;
        }
    }
    return best_child;
}

double AI_Mcts_A::UCTScore(
    const QSharedPointer<MctsNode> &child,
    const QSharedPointer<MctsNode> &parent)
{
    // If any child node has not been visited yet, return a high value to
    // encourage exploration
    if (child->GetVisitedNum() == 0)
    {
        return std::numeric_limits<double>::max();
    }
    else
    {
        // Otherwise, calculate the UCT score using the UCT formula.
        return static_cast<double>(
            child->GetWinsNum()) / child->GetVisitedNum() +
            ecf * qSqrt(qLn(parent->GetVisitedNum()) / child->GetVisitedNum()
        );
    }
}

HexAttacker AI_Mcts_A::SimulatePlayout(const QSharedPointer<MctsNode> &node, HexMatch board)
{
    // Start the simulation with the player at the node's move
    HexAttacker currentAttacker = node->GetAttacker();
    // Make the move at the node to make random moves from it
    auto [_r, _c] = node->GetMove();
    board(_r, _c) = static_cast<HexCell>(currentAttacker);
    // Continue simulation until a winner is detected
    while (!board.WinnerDecided(currentAttacker))
    {
        // Switch player
        currentAttacker = !currentAttacker;
        // Get valid moves
        QVector<HexPoint> validMoves = GetValidMoves(board);
        // Generate a distribution and choose a move randomly
        auto [__r, __c] = validMoves[random.bounded(validMoves.size())];
        board(__r, __c) = static_cast<HexCell>(currentAttacker);
    }
    return currentAttacker;
}

QVector<HexAttacker> AI_Mcts_A::ParallelPlayout(QSharedPointer<MctsNode> node,
    const HexMatch &board,
    uint number_of_threads)
{
    QVector<std::thread*> threads;
    QVector<HexAttacker> results(number_of_threads);
    for (uint thread_i = 0; thread_i < number_of_threads; thread_i++)
    {
        threads.push_back(new std::thread([&, thread_i]() {
            results[thread_i] = SimulatePlayout(node, board);
        }));
    }
    // Join the threads
    for (auto& thread : threads)
    {
        thread->join();
    }
    for (auto& thread : threads)
    {
        delete thread;
    }
    return results;
}

void AI_Mcts_A::Backpropagate(QSharedPointer<MctsNode> &node, HexAttacker winner)
{
    // Start backpropagation from the given node
    QSharedPointer<MctsNode> currentCode = node;
    while (currentCode != nullptr)
    {
        // Increment the node's visit count
        currentCode->Visit();
        // If the winner is the same as the player at the node, increment the node's
        // win count
        if (winner == currentCode->GetAttacker())
        {
            currentCode->Win();
        }
        // Move to the parent node for the next iteration
        currentCode = currentCode->GetParent();
    }
}

QSharedPointer<MctsNode> AI_Mcts_A::BestChild()
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

QVector<HexPoint> AI_Mcts_A::GetValidMoves(const HexMatch &board)
{
    QVector<HexPoint> validMoves;
    for (int i = 0, end = board.GetOrder(); i < end; i++)
    {
        for (int j = 0; j < end; j++)
        {
            if (board(i, j) == HexCell::Empty)
            {
                validMoves.push_back(qMakePair(i, j));
            }
        }
    }
    return validMoves;
}

