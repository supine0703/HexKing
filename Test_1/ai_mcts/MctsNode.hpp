#ifndef MCTSNODE_H
#define MCTSNODE_H

#include <QVector>
#include <QSharedPointer>
#include "HexArrayT.hpp"
#include "HexLocation.hpp"
#include "HexCell.hpp"

class MctsNode
{
    MctsNode(const MctsNode&) = delete;
    MctsNode& operator=(MctsNode&) = delete;
public:

    MctsNode(
        const HexAttacker& attacker,
        const HexLocation& move,
        const uint16_t& childrenTotal,
        const QSharedPointer<MctsNode>& parent = nullptr,
        const HexArrayT<bool>* sets = nullptr
    );

    HexAttacker Attacker() const;
    HexLocation Move() const;
    QSharedPointer<MctsNode> Parent() const;
    int WinsNum() const;
    int VisitsNum() const;
    int ExpandedNum() const;
    int ChildrenTotal() const;
    void Win();
    void Visit();

    void Expand(MctsNode* child);
    QSharedPointer<MctsNode> Child(const int& index);
    bool Get(int r, int c);
    HexArrayT<bool>& Get();
    void Set(QVector<HexLocation>& moves);
    void Set(int index);

private:
    const HexAttacker attacker;
    const HexLocation move;
    uint16_t childrenTotal;
    const QSharedPointer<MctsNode> parent;
    QAtomicInteger<uint32_t> winsNum; // number of wins
    QAtomicInteger<uint32_t> visitsNum; // number of visits
    QAtomicInteger<uint16_t> expandedNum; // number of expanded child
    QVector<QSharedPointer<MctsNode>> children;
    HexArrayT<bool> sets;
};

inline MctsNode::MctsNode(
    const HexAttacker& attacker,
    const HexLocation& move,
    const uint16_t& childrenTotal,
    const QSharedPointer<MctsNode>& parent,
    const HexArrayT<bool>* sets)
    : attacker(attacker)
    , move(move)
    , childrenTotal(childrenTotal)
    , parent(parent)
    , winsNum(0)
    , visitsNum(0)
    , expandedNum(0)
    , children(childrenTotal)
    , sets(sets ? *sets : 121)
{
}

inline HexAttacker MctsNode::Attacker() const
{
    return attacker;
}

inline HexLocation MctsNode::Move() const
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
    Q_ASSERT(index < childrenTotal);
    Q_ASSERT(index < expandedNum);
    return children[index];
}

inline bool MctsNode::Get(int r, int c)
{
    return sets[c + r * 11];
}

inline HexArrayT<bool> &MctsNode::Get()
{
    return sets;
}

inline void MctsNode::Set(QVector<HexLocation> &moves)
{
    for (const auto& m : moves)
    {
        sets[m.col + m.row * 11] = true;
    }
}

inline void MctsNode::Set(int index)
{
    sets[index] = true;
}

#endif // MCTSNODE_H
