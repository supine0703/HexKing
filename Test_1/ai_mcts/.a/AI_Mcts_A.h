#ifndef AI_MCTS_A_H
#define AI_MCTS_A_H

#include "HexAI.h"
#include <QPair>
#include <QTime>
#include <QMutex>
#include <QVector>
#include <QElapsedTimer>
#include <QSharedPointer>
#include <QRandomGenerator>

class MctsNode;

class AI_Mcts_A : public HexAI
{
public:
    // exploration constant factor, max decision time, is parallelized
    AI_Mcts_A(double ecf, qint64 max_decision_time, bool parallelized = false);
    ~AI_Mcts_A() { exit = true; };
    HexPoint ChooseMove(const HexMatch& board,  HexAttacker attacker) override;

private:
    void ExpandNode(const QSharedPointer<MctsNode>& node, const HexMatch& board);

    void MtcsSearch(
        int& itCounter,
        const HexMatch& board,
        uint number_of_threads
    );

    QSharedPointer<MctsNode> SelectChildPlayout(const QSharedPointer<MctsNode>& parent);

    double UCTScore(
        const QSharedPointer<MctsNode>& child,
        const QSharedPointer<MctsNode>& parent
    );

    HexAttacker SimulatePlayout(const QSharedPointer<MctsNode>& node, HexMatch board);

    QVector<HexAttacker> ParallelPlayout(
        QSharedPointer<MctsNode> node,
        const HexMatch& board,
        uint number_of_threads
    );

    void Backpropagate(QSharedPointer<MctsNode>& node, HexAttacker winner);

    QSharedPointer<MctsNode> BestChild();

    QVector<HexPoint> GetValidMoves(const HexMatch& board);

    double ecf;
    QElapsedTimer usedTime;
    qint64 endTime;
    bool parallelized;
    QRandomGenerator random;
    QSharedPointer<MctsNode> root;

    bool exit = false;
};

#endif // AI_MCTS_A_H
