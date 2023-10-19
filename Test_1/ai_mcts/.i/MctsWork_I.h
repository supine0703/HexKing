#ifndef MCTSWORK_I_H
#define MCTSWORK_I_H

#include <QRunnable>
#include <QSharedPointer>
#include "HexMatrix.hpp"
#include "MctsNode_I.hpp"
#include "MoWork.h"

QVector<HexLocation> GetValidMoves_I(const HexMatrix& board);

class QElapsedTimer;

class MctsWork_I : public QRunnable
{
public:
    MctsWork_I(
        const QSharedPointer<MctsNode_I>& root,
        const HexMatrix& board,
        const QElapsedTimer& usedTime,
        const uint& endTime,
        const double& ecf,
        MoWork& mow
    );
    void run() override;

private:
    double UCTScore(double cWins, double cVisits, double pVisits);
    void SelectChildPlayout(HexMatrix& virBoard, QSharedPointer<MctsNode_I>& node);
    void ExpandNode(QSharedPointer<MctsNode_I> &node, HexMatrix &virBoard);
    void SimulatedPlayout(const QSharedPointer<MctsNode_I>& node, HexMatrix& virBoard);
    void Backpropagate(QSharedPointer<MctsNode_I>& node);

    void CutTrees(HexMatrix& virBoard, QSharedPointer<MctsNode_I>& node);

    QSharedPointer<MctsNode_I> root;
    const HexMatrix& board;
    const QElapsedTimer& usedTime;
    const uint& endTime;
    const double ecf;
    HexAttacker nowAttacker;
    MoWork& m_mow;
};

#endif // MCTSWORK_I_H
