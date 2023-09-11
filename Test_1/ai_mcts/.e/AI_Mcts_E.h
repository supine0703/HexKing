#ifndef AI_MCTS_E_H
#define AI_MCTS_E_H

#include <QObject>
#include <QSharedPointer>
//#include "MctsNode.h"
//#include "MctsWork.h"
#include "HexMatch.hpp"

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

//    void ThisMatchEnd();

//    void ExpandNode(const QSharedPointer<MctsNode>& node, const HexMatch& board);

//    QSharedPointer<MctsNode> SelectChildPlayout(const QSharedPointer<MctsNode>& parent);

//    double UCTScore(
//        const QSharedPointer<MctsNode>& child,
//        const QSharedPointer<MctsNode>& parent
//    );

//    int _Num;
//    const QVector<QThread*>& threads;
//    QVector<SimulatePlayout*> works;
    double ecf;
    int endTime;
    bool parallelized;
//    QAtomicInt workCount;
    QElapsedTimer* usedTime;
    QSharedPointer<MctsNode> root;
//    bool isSleep = false;
//    QVector<MctsWork*> works;
signals:
//    void Wake();
//    void Finished();
//    void Working(const QSharedPointer<MctsNode>& node, _Board board);
};

#endif // AI_MCTS_E_H
