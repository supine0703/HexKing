#ifndef AI_MCTS_E_H
#define AI_MCTS_E_H

#include <QObject>
#include <QSharedPointer>
#include "HexMatch.hpp"

class QThreadPool;
class QElapsedTimer;
class MctsNode;
class MctsWork;

class AI_Mcts_E : public QObject
{
    Q_OBJECT
public:
    // exploration constant factor, max decision time, is parallelized
    AI_Mcts_E(
        double ecf,
        int max_decision_time,
        bool parallelized = false,
        QObject* parent = nullptr
    );
    ~AI_Mcts_E();
    HexPoint ChooseMove(const HexMatch& board,  HexAttacker attacker);

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
