#ifndef AI_MCTS_I_H
#define AI_MCTS_I_H

#include <QSharedPointer>
#include "HexAI.hpp"

class QThreadPool;
class QElapsedTimer;
class MctsNode_I;
class MctsWork_I;
class MoWork;

class AI_Mcts_I : public HexAI
{
public:
    // exploration constant factor, max decision time, is parallelized
    AI_Mcts_I(double ecf, uint max_decision_time, bool parallelized = false);
    ~AI_Mcts_I() override;
    std::string Name() const override { return "AI_Mcts_I"; }
    HexLocation ChooseMove(const HexMatrix& board,  HexAttacker attacker) override;
    void StopWork() override;

private:
    void MctsSearch(const HexMatrix& board);

    QSharedPointer<MctsNode_I> BestChild();

    bool RootIteration(const HexMatrix& board);

    double ecf;
    uint endTime;
    uint copyTime;
    bool parallelized;
    QElapsedTimer* usedTime;
    QSharedPointer<MctsNode_I> root;
    QThreadPool *pool;
    MoWork* mow;
};

#endif // AI_MCTS_I_H
