#include "MctsWork.h"

#include <QRandomGenerator>

/**
 * @brief MctsWork::MctsWork
 * @param match
 * @property board
 *
 * @bug When board copy match could lead to QVector member of board unfinished
 * copy if match comes from const reference parameter of calling function, so
 * in class AI_Mcts_E I have used provisional member boardCopy copied const
 * reference parameter board.
 *
 * @bug If I don't do this, board[_r].at(_c) will overload beacuse there are
 * probably no members inside board[_r].
 */

MctsWork::MctsWork(
    const QSharedPointer<MctsNode> &child,
    const HexMatch &match,
    QObject *parent)
    : QObject(parent)
    , board(match)
    , node(child)
{
    this->setAutoDelete(true);
}

MctsWork::~MctsWork()
{
}

void MctsWork::run()
{
    Backpropagate(SimulatePlayout());
//    emit Finished();
}

HexAttacker MctsWork::SimulatePlayout()
{
    // Start the simulation with the player at the node's move
    HexAttacker currentAttacker = node->GetAttacker();
    // Make the move at the node to make random moves from it
    const auto [_r, _c] = node->GetMove();
    board[_r][_c] = static_cast<HexCell>(currentAttacker);
    // Continue simulation until a winner is detected
    while (!board.WinnerDecided(currentAttacker))
    {
        // Switch player
        currentAttacker = !currentAttacker;
        // Get valid moves
        QVector<HexPoint> validMoves = GetValidMoves(board);
        // Generate a distribution and choose a move randomly
        static QRandomGenerator random;
        const auto [__r, __c] = validMoves[random.bounded(validMoves.count())];
        board[__r][__c] = static_cast<HexCell>(currentAttacker);
    }
    return currentAttacker;
}

void MctsWork::Backpropagate(HexAttacker winner)
{
    // Start backpropagation from the given node
    QSharedPointer<MctsNode> currentCode = node;
    while (currentCode != nullptr)
    {
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

QVector<HexPoint> GetValidMoves(const HexMatch &board)
{
    QVector<HexPoint> validMoves;
    for (int i = 0, end = board.Order(); i < end; i++)
    {
        for (int j = 0; j < end; j++)
        {
            if (board[i][j] == HexCell::Empty)
            {
                validMoves.push_back({i, j});
            }
        }
    }
    return validMoves;
}
