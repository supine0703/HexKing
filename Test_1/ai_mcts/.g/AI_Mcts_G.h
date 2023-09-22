#ifndef AI_MCTS_G_H
#define AI_MCTS_G_H

#include <QSharedPointer>
#include "HexAI.hpp"

class QThreadPool;
class QElapsedTimer;
class MctsNode;
class MctsWork;

class AI_Mcts_G : public HexAI
{
public:
    // exploration constant factor, max decision time, is parallelized
    AI_Mcts_G(double ecf, int max_decision_time, bool parallelized = false);
    ~AI_Mcts_G() override;
    QString Name() override { return "AI_Mcts_G"; }
    HexPoint ChooseMove(const HexMatch& board,  HexAttacker attacker) override;

private:

    void ExpandNode(const HexMatch& board);
    
    void MctsSearch(int& itCounter, const HexMatch& board);

    QSharedPointer<MctsNode> SelectChildPlayout();

    double UCTScore(const QSharedPointer<MctsNode>& child);

    QSharedPointer<MctsNode> BestChild();

    double ecf;
    int endTime;
    bool parallelized;
    QElapsedTimer* usedTime;
    QSharedPointer<MctsNode> root;
    QThreadPool *pool;

    bool exit = false;
    short stepNum = 0;
};

#endif // AI_MCTS_G_H
