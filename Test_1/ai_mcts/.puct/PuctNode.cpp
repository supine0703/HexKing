#include "PuctNode.hpp"

#define CPUCT 0.9

PuctNode::PuctNode(
    const HexAttacker &attacker,
    const HexLocation &move,
    const QSharedPointer<PuctNode>& parent,
    double prior_p)
    : attacker(attacker)
    , move(move)
    , parent(parent)
    , value_P(prior_p)
    , value_N(0)
    , value_Q(0)
    , value_U(0)
    , tensor(121)
{
    if (parent != nullptr && parent->parent != nullptr)
    {
        tensor = parent->parent->tensor;
        tensor.setBit(move.col * 11 + move.row);
    }
}

PuctNode::PuctNode(bool attacker, int move, const QSharedPointer<PuctNode>& parent, double prior_p)
    : attacker(static_cast<HexAttacker>(attacker))
    , move(move / 11, move % 11)
    , parent(parent)
    , value_P(prior_p)
    , value_N(0)
    , value_Q(0)
    , value_U(0)
    , tensor(121)
{
    if (parent != nullptr && parent->parent != nullptr)
    {
        tensor = parent->parent->tensor;
        tensor.setBit(move);
    }
}

void PuctNode::Expand(const QSharedPointer<PuctNode>& self, double priors[])
{
    QBitArray legalMove = ~(tensor | parent->tensor);
    for (int i = 0; i < legalMove.size(); i++)
    {
        if (legalMove[i])
        {
            children.append(QSharedPointer<PuctNode>(
                new PuctNode(*!attacker, i, self, priors[i])
            ));
        }
    }
}

double PuctNode::GetValue()
{
    value_U = CPUCT * value_P * qSqrt(parent->value_N) / (1 + value_N);
    return value_U + value_Q;
}

int PuctNode::Select()
{
    int index = -1;
    double maxValue = -1;
    for (int i = 0, end = children.length(); i < end; i++)
    {
        double value = children[i]->GetValue();
        if (value > maxValue)
        {
            maxValue = value;
            index = i;
        }
    }
    Q_ASSERT(index >= 0);
    return index;
}

void PuctNode::Update()
{
    value_N++;
    value_Q += (value_P - value_Q) / value_N;
    QSharedPointer<PuctNode> node = parent;
    while (node != nullptr)
    {
        node->value_N++;
        node->value_Q += (value_P - node->value_Q) / node->value_N;
        node = node->parent;
    }
}



