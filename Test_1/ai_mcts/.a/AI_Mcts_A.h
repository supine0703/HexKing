#ifndef AI_MCTS_A_H
#define AI_MCTS_A_H

#include "HexAI.hpp"
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
    std::string Name() const override { return "AI_Mcts_A"; }
    HexLocation ChooseMove(const HexMatrix& board,  HexAttacker attacker) override;
    void StopWork() override;

private:
    void ExpandNode(const QSharedPointer<MctsNode>& node, const HexMatrix& board);

    void MtcsSearch(int& itCounter,
                    const HexMatrix &board,
        uint number_of_threads
    );

    QSharedPointer<MctsNode> SelectChildPlayout(const QSharedPointer<MctsNode>& parent);

    double UCTScore(
        const QSharedPointer<MctsNode>& child,
        const QSharedPointer<MctsNode>& parent
    );
    
    HexAttacker SimulatePlayout(const QSharedPointer<MctsNode>& node, HexMatrix board);

    QVector<HexAttacker> ParallelPlayout(
        QSharedPointer<MctsNode> node,
        const HexMatrix& board,
        uint number_of_threads
    );

    void Backpropagate(QSharedPointer<MctsNode>& node, HexAttacker winner);

    QSharedPointer<MctsNode> BestChild();
    
    QVector<HexLocation> GetValidMoves(const HexMatrix& board);

    double ecf;
    QElapsedTimer usedTime;
    qint64 endTime;
    bool parallelized;
    QRandomGenerator random;
    QSharedPointer<MctsNode> root;

    bool exit = false;
};

#endif // AI_MCTS_A_H
