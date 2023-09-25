#ifndef MCTSWORK_E_H
#define MCTSWORK_E_H

#include <QRunnable>
#include <QSharedPointer>
#include "HexBoard.hpp"
#include "MctsNode.hpp"

QVector<HexPoint> GetValidMoves_E(const HexBoard& board);

class MctsWork_E : public QRunnable
{
public:
    MctsWork_E(const QSharedPointer<MctsNode>& child, const HexBoard& board);
    ~MctsWork_E();
    void run() override;

private:
    void SimulatedPlayout();

    HexBoard board;
    QSharedPointer<MctsNode> node;
};

#endif // MCTSWORK_E_H
