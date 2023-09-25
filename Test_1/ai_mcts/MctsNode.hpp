#ifndef MCTSNODE_H
#define MCTSNODE_H

#include <QVector>
#include <QSharedPointer>
#include "HexPoint.hpp"
#include "HexCell.hpp"

class MctsNode
{
    MctsNode(const MctsNode&) = delete;
    MctsNode& operator=(MctsNode&) = delete;
public:
    MctsNode(
        const HexAttacker& attacker,
        const HexPoint& move,
        const uint16_t& childrenTotal,
        const QSharedPointer<MctsNode>& parent = nullptr
    );

    HexAttacker Attacker() const;
    HexPoint Move() const;
    QSharedPointer<MctsNode> Parent() const;
    int WinsNum() const;
    int VisitsNum() const;
    int ExpandedNum() const;
    int ChildrenTotal() const;
    void Win();
    void Visit();

    void Expand(MctsNode* child);
    QSharedPointer<MctsNode> Child(const int& index);

private:
    const HexAttacker attacker;
    const HexPoint move;
    const uint16_t childrenTotal;
    const QSharedPointer<MctsNode> parent;
    QAtomicInteger<uint32_t> winsNum; // number of wins
    QAtomicInteger<uint32_t> visitsNum; // number of visits
    QAtomicInteger<uint16_t> expandedNum; // number of expanded child
    QVector<QSharedPointer<MctsNode>> children;
};

inline MctsNode::MctsNode(
    const HexAttacker& attacker,
    const HexPoint& move,
    const uint16_t& childrenTotal,
    const QSharedPointer<MctsNode>& parent)
    : attacker(attacker)
    , move(move)
    , childrenTotal(childrenTotal)
    , parent(parent)
    , winsNum(0)
    , visitsNum(0)
    , expandedNum(0)
    , children(childrenTotal)
{
}

inline HexAttacker MctsNode::Attacker() const
{
    return attacker;
}

inline HexPoint MctsNode::Move() const
{
    return move;
}

inline QSharedPointer<MctsNode> MctsNode::Parent() const
{
    return parent;
}

inline int MctsNode::WinsNum() const
{
    return winsNum;
}

inline int MctsNode::VisitsNum() const
{
    return visitsNum;
}

inline int MctsNode::ExpandedNum() const
{
    return expandedNum;
}

inline int MctsNode::ChildrenTotal() const
{
    return childrenTotal;
}

inline void MctsNode::Win()
{
    ++winsNum;
}

inline void MctsNode::Visit()
{
    ++visitsNum;
}

inline void MctsNode::Expand(MctsNode *child)
{
    children[expandedNum].reset(child);
    ++expandedNum;
}

inline QSharedPointer<MctsNode> MctsNode::Child(const int &index)
{
    Q_ASSERT(index < expandedNum);
    return children[index];
}

#endif // MCTSNODE_H
