#ifndef MCTSWORK_HPP
#define MCTSWORK_HPP

#include <QRunnable>
#include <QSharedPointer>
#include "HexMatch.hpp"
#include "MctsNode.hpp"

QVector<HexPoint> GetValidMoves(const HexMatch& board);

class MctsWork : public QRunnable
{
public:
    MctsWork(const QSharedPointer<MctsNode>& child, const HexMatch& match);
    ~MctsWork();
    void run() override;

private:
    void SimulatedPlayout();

    HexMatch board;
    QSharedPointer<MctsNode> node;
};

#endif // MCTSWORK_HPP
