#ifndef MCTSNODE_H
#define MCTSNODE_H

#include <QVector>
#include <QSharedPointer>
#include "HexMatch.hpp"

class MctsNode
{
    MctsNode(const MctsNode&) = delete;
    MctsNode& operator=(MctsNode&) = delete;
public:
    MctsNode(
        const HexAttacker &attacker, 
        const HexPoint &move, 
        const QSharedPointer<MctsNode> &parent = nullptr
    );

    HexAttacker GetAttacker() const;
    HexPoint GetMove() const;
    QSharedPointer<MctsNode> GetParent() const;
    int GetWinsNum() const;
    int GetVisitedNum() const;
    void Win();
    void Visit();

    QVector<QSharedPointer<MctsNode>> children;

private:
    const HexAttacker attacker;
    const HexPoint move;
    const QSharedPointer<MctsNode> parent;
    QAtomicInt winsNum; // number of wins
    QAtomicInt visitedNum; // number of visited
};

inline MctsNode::MctsNode(
    const HexAttacker &attacker,
    const HexPoint &move,
    const QSharedPointer<MctsNode> &parent)
    : winsNum(0)
    , visitedNum(0)
    , attacker(attacker)
    , move(move)
    , parent(parent)
{
}

inline HexAttacker MctsNode::GetAttacker() const
{
    return attacker;
}

inline HexPoint MctsNode::GetMove() const
{
    return move;
}

inline QSharedPointer<MctsNode> MctsNode::GetParent() const
{
    return parent;
}

inline int MctsNode::GetWinsNum() const
{
    return winsNum;
}

inline int MctsNode::GetVisitedNum() const
{
    return visitedNum;
}

inline void MctsNode::Win()
{
    winsNum++;
}

inline void MctsNode::Visit()
{
    visitedNum++;
}

#endif // MCTSNODE_H
