#ifndef AI_MCTS_E_H
#define AI_MCTS_E_H

#include <QSharedPointer>
#include "HexAI.hpp"

class QThreadPool;
class QElapsedTimer;
class MctsNode;
class MctsWork_E;

class AI_Mcts_E : public HexAI
{
public:
    // exploration constant factor, max decision time, is parallelized
    AI_Mcts_E(double ecf, int max_decision_time, bool parallelized = false);
    ~AI_Mcts_E() override;
    std::string Name() const override { return "AI_Mcts_E"; }
    HexLocation ChooseMove(const HexMatrix& board,  HexAttacker attacker) override;
    void StopWork() override;

private:
    
    void ExpandNode(const HexMatrix& board);
    
    void MctsSearch(int& itCounter, const HexMatrix& board);

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
