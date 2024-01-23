#ifndef PY_MCTS_HPP
#define PY_MCTS_HPP

#include "HexAI.hpp"
#include <QObject>

class QElapsedTimer;
class PMoWork;

class PyMcts : public HexAI
{
public:
    PyMcts(double multiple = 0.01, int time = 1, bool useMo = false);
    ~PyMcts() override;
    std::string Name() const override { return "PY_MCTS"; }
    void StopWork() override;
    void Exit() override;
    void Init() override;

    HexLocation ChooseMove(const HexMatrix &board, HexAttacker attacker) override;

private:
    const bool useMo;
    const uint gTime;
    uint itNum;
    QElapsedTimer* usedTime = nullptr;
    uint64_t totalTime = 0;
    PMoWork* mow;

    bool isExit = false;
    uint runTime = 5000;

    void init();
};


#endif // PY_MCTS_HPP
