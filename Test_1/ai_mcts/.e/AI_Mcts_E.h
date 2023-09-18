#ifndef AI_MCTS_E_H
#define AI_MCTS_E_H

#include <QSharedPointer>
#include "HexAI.h"

class QThreadPool;
class QElapsedTimer;
class MctsNode;
class MctsWork;

class AI_Mcts_E : public HexAI
{
public:
    // exploration constant factor, max decision time, is parallelized
    AI_Mcts_E(double ecf, int max_decision_time, bool parallelized = false);
    ~AI_Mcts_E() override;
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
};

#endif // AI_MCTS_E_H
