#ifndef PY_MCTS_HPP
#define PY_MCTS_HPP

#include "HexAI.hpp"

class QElapsedTimer;
class MoWork;

class PyMcts : public HexAI
{
public:
    PyMcts(bool useMo = false);
    ~PyMcts() override;
    std::string Name() const override { return "PY_MCTS"; }
    void StopWork() override { };

    HexLocation ChooseMove(const HexMatrix &board, HexAttacker attacker) override;

private:
    const bool useMo;
    QElapsedTimer* usedTime = nullptr;
    uint64_t totalTime = 0;
    MoWork* mow;


    void init();
};


#endif // PY_MCTS_HPP
