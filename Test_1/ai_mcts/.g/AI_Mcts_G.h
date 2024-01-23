#ifndef AI_MCTS_G_H
#define AI_MCTS_G_H

#include <QSharedPointer>
#include "HexAI.hpp"

class QThreadPool;
class QElapsedTimer;
class MctsNode;
class MctsWork_G;

class AI_Mcts_G : public HexAI
{
public:
    // exploration constant factor, max decision time, is parallelized
    AI_Mcts_G(double ecf, uint max_decision_time, bool parallelized = false);
    ~AI_Mcts_G() override;
    std::string Name() const override { return "AI_Mcts_G"; }
    HexLocation ChooseMove(const HexMatrix& board,  HexAttacker attacker) override;
    void StopWork() override;

private:
    void MctsSearch(const HexMatrix& board);

    QSharedPointer<MctsNode> BestChild();
    
    bool RootIteration(const HexMatrix& board);

    double ecf;
    uint endTime;
    uint copyTime;
    bool parallelized;
    QElapsedTimer* usedTime;
    QSharedPointer<MctsNode> root;
    QThreadPool *pool;
};

#endif // AI_MCTS_G_H
