#include "MctsWork_E.h"

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
 * @bug If I don't do this, board[_r].at(_c) will overflow beacuse there are
 * probably no members inside board[_r].
 *
 * @resolve I guess the problem may come from QVector beacuse the bug didn't
 * reappear after I replaced QVector with a self-defined array.
 */

MctsWork_E::MctsWork_E(const QSharedPointer<MctsNode>& child, const HexMatrix &board)
    : board(board)
    , node(child)
{
//    this->setAutoDelete(true); // default = true
}

MctsWork_E::~MctsWork_E()
{
}

void MctsWork_E::run()
{
    SimulatedPlayout();
}

void MctsWork_E::SimulatedPlayout()
{
    // Start the simulation with the player at the node's move
    HexAttacker currentAttacker = node->Attacker();
    // Make the move at the node to make random moves from it
    const auto [_r, _c] = node->Move();
    board(_r, _c, currentAttacker);
    // Continue simulation until a winner is detected
    bool WinnerDecided(const HexMatrix& board,const HexAttacker& attacker);
    while (!WinnerDecided(board, currentAttacker))
    {
        // Switch player
        currentAttacker = !currentAttacker;
        // Get valid moves
        QVector<HexLocation> validMoves = GetValidMoves_E(board);
        // Generate a distribution and choose a move randomly
        static QRandomGenerator random;
        const auto [__r, __c] = validMoves[random.bounded(validMoves.count())];
        board(__r, __c, currentAttacker);
    }

    // Start backpropagation from the given node
    QSharedPointer<MctsNode> currentCode = node;
    while (currentCode != nullptr)
    {
        currentCode->Visit();
        // If the winner is the same as the player at the node, increment the node's
        // win count
        if (currentAttacker == currentCode->Attacker())
        {
            currentCode->Win();
        }
        // Move to the parent node for the next iteration
        currentCode = currentCode->Parent();
    }
}

QVector<HexLocation> GetValidMoves_E(const HexMatrix &board)
{
    QVector<HexLocation> validMoves;
    for (int i = 0, end = board.Order(); i < end; i++)
    {
        for (int j = 0; j < end; j++)
        {
            if (board(i, j) == HexCell::Empty)
            {
                validMoves.push_back({i, j});
            }
        }
    }
    return validMoves;
}

#include <QStack>
inline bool WinnerDecided(const HexMatrix& board,const HexAttacker& attacker)
{
    int num[2] { 0, 0 };
    int &r = num[0];
    int &c = num[1];
    
    QStack<HexLocation> stack;
    QVector<HexLocation> visited;
    int order = board.Order();
    for (int &p = num[*(!attacker)]; p < order; p++)
    {
        if (board(r, c) == attacker)
        {
            stack.push({r, c});
        }
    }

    // dfs
    while (!stack.isEmpty())
    {
        HexLocation coord = stack.pop();
        r = coord.row;
        c = coord.col;

        if (visited.contains({r, c}))
        {
            continue;
        }

        visited.push_back({r, c});

        // win
        if (num[*attacker] == order - 1)
        {
            return true;
        }
        // push next possible point
        int r_add = r + 1, r_sub = r - 1, c_add = c + 1, c_sub = c - 1;
        if (r > 0)
        {
            if (board(r_sub, c) == attacker)
            {
                stack.push({r_sub, c});
            }
            if (c < order - 1 && board(r_sub, c_add) == attacker)
            {
                stack.push({r_sub, c_add});
            }
        }
        if (r_add < order)
        {
            if (board(r_add, c) == attacker)
            {
                stack.push({r_add, c});
            }
            if (c > 0 && board(r_add, c_sub) == attacker)
            {
                stack.push({r_add, c_sub});
            }
        }
        if (c > 0 && board(r, c_sub) == attacker)
        {
            stack.push({r, c_sub});
        }
        if (c_add < order && board(r, c_add) == attacker)
        {
            stack.push({r, c_add});
        }
    }
    return false;
}
