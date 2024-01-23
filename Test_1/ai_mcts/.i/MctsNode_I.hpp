#ifndef MCTSNODE_I_HPP
#define MCTSNODE_I_HPP

#include <QVector>
#include <QSharedPointer>
#include "HexArrayT.hpp"
#include "HexLocation.hpp"
#include "HexCell.hpp"

class MctsNode_I
{
    MctsNode_I(const MctsNode_I&) = delete;
    MctsNode_I& operator=(MctsNode_I&) = delete;
public:

    bool cutFlag = true;

    MctsNode_I(
        const HexAttacker& attacker,
        const HexLocation& move,
        const uint16_t& childrenTotal,
        const QSharedPointer<MctsNode_I>& parent = nullptr,
        const HexArrayT<bool>* sets = nullptr
    );

    HexAttacker Attacker() const;
    HexLocation Move() const;
    QSharedPointer<MctsNode_I> Parent() const;
    uint WinsNum() const;
    uint VisitsNum() const;
    uint ExpandedNum() const;
    uint ChildrenTotal() const;
    void Win();
    void Visit();

    void Expand(int index, MctsNode_I* child);
    QSharedPointer<MctsNode_I>& Child(const int& index);
    bool Allow(int index);
    bool Get(int r, int c);
    HexArrayT<bool>& Get();
    void Set(QVector<HexLocation>& moves);
    void Set(int index);
    void Cut(int index);
    void Clean();

private:
    const HexAttacker attacker;
    const HexLocation move;
    uint16_t childrenTotal;
    const QSharedPointer<MctsNode_I> parent;
    QAtomicInteger<uint32_t> winsNum; // number of wins
    QAtomicInteger<uint32_t> visitsNum; // number of visits
    QAtomicInteger<uint16_t> expandedNum; // number of expanded child
    QVector<QSharedPointer<MctsNode_I>> children;
    HexArrayT<bool> sets;
};

inline MctsNode_I::MctsNode_I(
    const HexAttacker& attacker,
    const HexLocation& move,
    const uint16_t& childrenTotal,
    const QSharedPointer<MctsNode_I>& parent,
    const HexArrayT<bool>* sets)
    : attacker(attacker)
    , move(move)
    , childrenTotal(childrenTotal)
    , parent(parent)
    , winsNum(0)
    , visitsNum(0)
    , expandedNum(0)
    , children(121)
    , sets(sets ? *sets : 121)
{
}

inline HexAttacker MctsNode_I::Attacker() const
{
    return attacker;
}

inline HexLocation MctsNode_I::Move() const
{
    return move;
}

inline QSharedPointer<MctsNode_I> MctsNode_I::Parent() const
{
    return parent;
}

inline uint MctsNode_I::WinsNum() const
{
    return winsNum;
}

inline uint MctsNode_I::VisitsNum() const
{
    return visitsNum;
}

inline uint MctsNode_I::ExpandedNum() const
{
    return expandedNum;
}

inline uint MctsNode_I::ChildrenTotal() const
{
    return childrenTotal;
}

inline void MctsNode_I::Win()
{
    ++winsNum;
}

inline void MctsNode_I::Visit()
{
    ++visitsNum;
}

inline void MctsNode_I::Expand(int index, MctsNode_I *child)
{
    Q_ASSERT(sets[index]);
    children[index].reset(child);
    ++expandedNum;
}

inline QSharedPointer<MctsNode_I>& MctsNode_I::Child(const int &index)
{
    Q_ASSERT(index < 121);
    Q_ASSERT(index < 121);
    return children[index];
}

inline bool MctsNode_I::Allow(int index)
{
    return sets[index];
}

inline bool MctsNode_I::Get(int r, int c)
{
    return sets[c + r * 11];
}

inline HexArrayT<bool> &MctsNode_I::Get()
{
    return sets;
}

inline void MctsNode_I::Set(QVector<HexLocation> &moves)
{
    for (const auto& m : moves)
    {
        sets[m.col + m.row * 11] = true;
    }
}

inline void MctsNode_I::Set(int index)
{
    sets[index] = true;
}

inline void MctsNode_I::Cut(int index)
{
    if (sets[index])
    {
        sets[index] = false;
        if (childrenTotal > 0) --childrenTotal;
        if (expandedNum > 0) --expandedNum;
    }
}

inline void MctsNode_I::Clean()
{
    for (auto& c : children)
    {
        c->Clean();
    }
    for (int i = 0; i < 121; i++)
    {
        if (!sets[i])
        {
            children[i].clear();
        }
    }
}



#endif // MCTSNODE_I_HPP
