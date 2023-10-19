#include "MctsWork_I.h"
#include "qdebug.h"

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

MctsWork_I::MctsWork_I(
    const QSharedPointer<MctsNode_I>& root,
    const HexMatrix& board,
    const QElapsedTimer& usedTime,
    const uint& endTime,
    const double& ecf,
    MoWork& mow)
    : root(root)
    , board(board)
    , usedTime(usedTime)
    , endTime(endTime)
    , ecf(ecf)
    , m_mow(mow)
{
    this->setAutoDelete(true); // default = true
}

void MctsWork_I::run()
{
    while (usedTime.elapsed() < endTime)
    {
        HexMatrix virBoard(board);
        QSharedPointer<MctsNode_I> node = root;
        nowAttacker = !node->Attacker();
        SelectChildPlayout(virBoard, node);
        ExpandNode(node, virBoard);
        SimulatedPlayout(node, virBoard);
        Backpropagate(node);
    }
}

double MctsWork_I::UCTScore(double cWins, double cVisits, double pVisits)
{
    // Otherwise, calculate the UCT score using the UCT formula.
    return cWins / cVisits + ecf * qSqrt(qLn(pVisits) / cVisits);
}

void MctsWork_I::SelectChildPlayout(HexMatrix& virBoard, QSharedPointer<MctsNode_I>& node)
{
    while (node->ExpandedNum() == node->ChildrenTotal())
    {
        if (node->cutFlag)
        {
            CutTrees(virBoard, node);
        }
        int count = 0;
        Q_ASSERT(node->ChildrenTotal()!=0);
        int selectIndex = -1;
        double maxScore = 0;
        uint nVisits = node->VisitsNum();
        for (int i = 0; i < 121; i++)
        {
            if (!node->Allow(i)) continue;
            if (!node->Child(i)->VisitsNum() && selectIndex == -1)
            {
                selectIndex = i;
                continue;
            }
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
        Q_ASSERT(selectIndex >= 0);
        node = node->Child(selectIndex);
        if (node->ChildrenTotal() == 0)
        {
            break;
        }
        virBoard(node->Move(), node->Attacker());
        nowAttacker = !nowAttacker;
    }
}

#include "ConstBoard.hpp"

bool WinnerDecided_G(const HexMatrix& board,const HexAttacker& attacker);
void MctsWork_I::ExpandNode(QSharedPointer<MctsNode_I>& node, HexMatrix& virBoard)
{
    if (WinnerDecided_G(virBoard, node->Attacker()) || node->ChildrenTotal() == 0)
    {
        return;
    }
    bool buff[121];
    for (int i = 0; i < 121; i++)
    {
        buff[i] = true;
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
    for (int i = 0; i < 121; i++)
    {
        if (!node->Child(i).isNull())
        {
            buff[i] = false;
        }
    }

    for (int i = 0; i < 121; i++)
    {
        if (virBoard(i) == HexCell::Empty && buff[i] && node->Allow(i))
        {
            HexArrayT<bool> sets(node->Get());
            sets[i] = false;
            int subNums = -1;
            for (BoardIterator p(ConstBoard::Get(11).Nbs((HexPoint)(i+7),3));
                 p; ++p)
            {
                if (*p >= 7 && sets[*p-7] == false && virBoard(*p-7) == HexCell::Empty)
                {
                    subNums++;
                    sets[*p-7] = true;
                }
            }

            int r = i / 11, c = i % 11;
            node->Expand(
                i,
                new MctsNode_I(nowAttacker, {r, c}, node->ChildrenTotal()+subNums, node, &sets)
            );
            node = node->Child(i);
            break;
        }
    }
    // }
    mutex.unlock();
}

bool WinnerDecided_G(const HexMatrix& board,const HexAttacker& attacker);
void MctsWork_I::SimulatedPlayout(
    const QSharedPointer<MctsNode_I>& node, HexMatrix& virBoard)
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
        QVector<HexLocation> validMoves = GetValidMoves_I(virBoard);
        // Generate a distribution and choose a move randomly
        static QRandomGenerator random;
        virBoard(validMoves[random.bounded(validMoves.count())], nowAttacker);
    }
}

void MctsWork_I::Backpropagate(QSharedPointer<MctsNode_I>& node)
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

void MctsWork_I::CutTrees(HexMatrix &virBoard, QSharedPointer<MctsNode_I> &node)
{
    for (int i = 0; i < 121; i++)
    {
        HexPoint point = (HexPoint)(i + 7);
        if (!node->Allow(i)) continue;
        QVector<HexPoint> distance1;
        QVector<HexPoint> distance2;
        for (BoardIterator p(ConstBoard::Get(11).Nbs(point,2));
             p; ++p)
        {
            if (*p > 7 && virBoard(*p-7) != HexCell::Empty)
            {
                if (ConstBoard::Get(11).Distance(point, *p) == 1)
                {
                    distance1.push_back(*p);
                }
                else // 2
                {
                    distance2.push_back(*p);
                }
            }
        }
        for (auto p : distance1)
        {
            if (HexPointUtil::isEdge(p) && distance1.count() == 1 && distance2.empty())
            {
                if (p == NORTH || p == SOUTH)
                {
                    node->Cut(p-7);
                    if ((p-7)%11) node->Cut(p-6);
                }
                else
                {
                    node->Cut(p-7);
                    if (p<117) node->Cut(p+4);// 121+7-11=117 11-7=4
                }
            }
            else if (distance2.empty())
            {
                node->Cut(p-7);
            }
        }

    }
    node->cutFlag = false;
}

QVector<HexLocation> GetValidMoves_I(const HexMatrix &board)
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
inline bool WinnerDecided_G(const HexMatrix& board,const HexAttacker& attacker)
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
