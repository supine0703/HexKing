#ifndef MCTSWORK_G_H
#define MCTSWORK_G_H

#include <QRunnable>
#include <QSharedPointer>
#include "HexMatrix.hpp"
#include "MctsNode.hpp"

QVector<HexLocation> GetValidMoves_G(const HexMatrix& board);

class QElapsedTimer;

class MctsWork_G : public QRunnable
{
public:
    MctsWork_G(
        const QSharedPointer<MctsNode>& root,
        const HexMatrix& board,
        const QElapsedTimer& usedTime,
        const uint& endTime,
        const double& ecf
    );
    void run() override;

private:
    double UCTScore(double cWins, double cVisits, double pVisits);
    void SelectChildPlayout(HexMatrix& virBoard, QSharedPointer<MctsNode>& node);
    void ExpandNode(QSharedPointer<MctsNode> &node, HexMatrix &virBoard);
    void SimulatedPlayout(const QSharedPointer<MctsNode>& node, HexMatrix& virBoard);
    void Backpropagate(QSharedPointer<MctsNode>& node);

    QSharedPointer<MctsNode> root;
    const HexMatrix& board;
    const QElapsedTimer& usedTime;
    const uint& endTime;
    const double ecf;
    HexAttacker nowAttacker;
};

#endif // MCTSWORK_G_H
