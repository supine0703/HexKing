#ifndef AI_MCTS_K_H
#define AI_MCTS_K_H

#include "HexAI.hpp"

class AI_Mcts_K : public HexAI
{
public:
    AI_Mcts_K();

    std::string Name() const override { return "Test_K"; }
    void StopWork() override;
    HexLocation ChooseMove(const HexMatrix &board, HexAttacker attacker) override;
};

#endif // AI_MCTS_K_H
