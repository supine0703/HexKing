#ifndef MCTSWORK_G_H
#define MCTSWORK_G_H

#include <QRunnable>
#include <QSharedPointer>
#include "HexBoard.hpp"
#include "MctsNode.hpp"

QVector<HexPoint> GetValidMoves_G(const HexBoard& board);

class QElapsedTimer;

class MctsWork_G : public QRunnable
{
public:
    MctsWork_G(
        const QSharedPointer<MctsNode>& root,
        const HexBoard& board,
        const QElapsedTimer& usedTime,
        const uint& endTime,
        const double& ecf
    );
    void run() override;

private:
    double UCTScore(double cWins, double cVisits, double pVisits);
    void SelectChildPlayout(HexBoard& virBoard, QSharedPointer<MctsNode>& node);
    void ExpandNode(QSharedPointer<MctsNode> &node, HexBoard &virBoard);
    void SimulatedPlayout(const QSharedPointer<MctsNode>& node, HexBoard& virBoard);
    void Backpropagate(QSharedPointer<MctsNode>& node);

    QSharedPointer<MctsNode> root;
    const HexBoard& board;
    const QElapsedTimer& usedTime;
    const uint& endTime;
    const double ecf;
    HexAttacker nowAttacker;
};

#endif // MCTSWORK_G_H
