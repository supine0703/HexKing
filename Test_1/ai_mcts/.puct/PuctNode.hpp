#ifndef PUCTNODE_HPP
#define PUCTNODE_HPP

#include "HexCell.hpp"
#include "HexLocation.hpp"
#include <QMutex>
#include <QSharedPointer>
#include <QVector>
#include <bitset>
#include <QBitArray>

class PuctNode
{
public:
    PuctNode(
        const HexAttacker& attacker,
        const HexLocation& move,
        const QSharedPointer<PuctNode>& parent,
        double prior_p
    );

    PuctNode(
        bool attacker,
        int move,
        const QSharedPointer<PuctNode>& parent,
        double prior_p
    );

    void Expand(const QSharedPointer<PuctNode>& self, double priors[]);

    double GetValue();

    int Select();

    void Update();

private:
    const HexAttacker attacker;
    const HexLocation move;
    const QSharedPointer<PuctNode> parent;
    QVector<QSharedPointer<PuctNode>> children;

    int value_N;            // 访问次数
    double value_U;         // 置信上限
    double value_Q;         // 平均价值
    const double value_P;   // 先验概率


    QBitArray tensor;

};

#endif // PUCTNODE_HPP
