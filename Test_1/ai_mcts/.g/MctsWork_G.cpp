#include "MctsWork_G.h"

#include <QRandomGenerator>
#include <QElapsedTimer>
#include <QMutex>
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

MctsWork_G::MctsWork_G(
    const QSharedPointer<MctsNode>& root,
    const HexBoard& board,
    const QElapsedTimer& usedTime,
    const uint& endTime,
    const double& ecf)
    : root(root)
    , board(board)
    , usedTime(usedTime)
    , endTime(endTime)
    , ecf(ecf)
{
    this->setAutoDelete(true); // default = true
}

void MctsWork_G::run()
{
    while (usedTime.elapsed() < endTime)
    {
        HexBoard virBoard(board);
        QSharedPointer<MctsNode> node = root;
        nowAttacker = !node->Attacker();
        SelectChildPlayout(virBoard, node);
        ExpandNode(node, virBoard);
        SimulatedPlayout(node, virBoard);
        Backpropagate(node);
    }
}

double MctsWork_G::UCTScore(double cWins, double cVisits, double pVisits)
{
    // Otherwise, calculate the UCT score using the UCT formula.
    return cWins / cVisits + ecf * qSqrt(qLn(pVisits) / cVisits);
}

void MctsWork_G::SelectChildPlayout(HexBoard& virBoard, QSharedPointer<MctsNode>& node)
{
    while (node->ExpandedNum() == node->ChildrenTotal())
    {
        int nVisits = node->VisitsNum();
        double maxScore = UCTScore(
            node->Child(0)->WinsNum(),
            node->Child(0)->VisitsNum(),
            nVisits);
        int selectIndex = 0;
        for (int i = 1, end = node->ChildrenTotal(); i < end; i++)
        {
            double uctScore = UCTScore(
                node->Child(i)->WinsNum(),
                node->Child(i)->VisitsNum(),
                nVisits);
            if (uctScore > maxScore)
            {
                maxScore = uctScore;
                selectIndex = i;
            }
        }
        node = node->Child(selectIndex);
        if (node->ChildrenTotal() == 0)
        {
            break;
        }
        virBoard(node->Move(), node->Attacker());
        nowAttacker = !nowAttacker;
    }
}

bool WinnerDecided_G(const HexBoard& board,const HexAttacker& attacker);
void MctsWork_G::ExpandNode(QSharedPointer<MctsNode>& node, HexBoard& virBoard)
{
    if (WinnerDecided_G(virBoard, node->Attacker()) || node->ChildrenTotal() == 0)
    {
        return;
    }
    int order = virBoard.Order();
    bool buff[order][order];
    for (int i = 0; i < order; i++)
    {
        for (int j = 0; j < order; j++)
        {
            buff[i][j] = true;
        }
    }
    static QMutex mutex;
    mutex.lock();
    // thread unsafe {
    while (node->ExpandedNum() == node->ChildrenTotal())
    {
        mutex.unlock();
        SelectChildPlayout(virBoard, node);
        mutex.lock();
    }
    int expandedNum = node->ExpandedNum();
    for (int i = 0; i < expandedNum; i++)
    {
        auto index = node->Child(i)->Move();
        buff[index.row][index.col] = false;
    }
    for (int i = 0; i < order; i++)
    {
        for (int j = 0; j < order; j++)
        {
            if (virBoard(i, j) == HexCell::Empty && buff[i][j])
            {
                node->Expand(
                    new MctsNode(nowAttacker, {i, j}, node->ChildrenTotal() - 1, node)
                );
                i = j = order;
            }
        }
    }
    // }
    mutex.unlock();
    node = node->Child(expandedNum);
}

bool WinnerDecided_G(const HexBoard& board,const HexAttacker& attacker);
void MctsWork_G::SimulatedPlayout(
    const QSharedPointer<MctsNode>& node, HexBoard& virBoard)
{
    if (WinnerDecided_G(virBoard, nowAttacker))
    {
        return;
    }
    if (WinnerDecided_G(virBoard, !nowAttacker))
    {
        nowAttacker = !nowAttacker;
        return;
    }
    // Make the move at the node to make random moves from it
    virBoard(node->Move(), nowAttacker);
    // Continue simulation until a winner is detected
    while (!WinnerDecided_G(virBoard, nowAttacker))
    {
        // Switch player
        nowAttacker = !nowAttacker;
        // Get valid moves
        QVector<HexPoint> validMoves = GetValidMoves_G(virBoard);
        // Generate a distribution and choose a move randomly
        static QRandomGenerator random;
        virBoard(validMoves[random.bounded(validMoves.count())], nowAttacker);
    }
}

void MctsWork_G::Backpropagate(QSharedPointer<MctsNode>& node)
{
    // Start backpropagation from the given node
    while (node != nullptr)
    {
        if (nowAttacker == node->Attacker())
        {
            node->Win();
        }
        node->Visit();
        node = node->Parent();
    }
}

QVector<HexPoint> GetValidMoves_G(const HexBoard &board)
{
    QVector<HexPoint> validMoves;
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
inline bool WinnerDecided_G(const HexBoard& board,const HexAttacker& attacker)
{
    int num[2] { 0, 0 };
    int &r = num[0];
    int &c = num[1];

    QStack<HexPoint> stack;
    QVector<HexPoint> visited;
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
        HexPoint coord = stack.pop();
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
