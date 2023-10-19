#ifndef MCTSWORK_E_H
#define MCTSWORK_E_H

#include <QRunnable>
#include <QSharedPointer>
#include "HexMatrix.hpp"
#include "MctsNode.hpp"

QVector<HexLocation> GetValidMoves_E(const HexMatrix& board);

class MctsWork_E : public QRunnable
{
public:
    MctsWork_E(const QSharedPointer<MctsNode>& child, const HexMatrix& board);
    ~MctsWork_E();
    void run() override;

private:
    void SimulatedPlayout();
    
    HexMatrix board;
    QSharedPointer<MctsNode> node;
};

#endif // MCTSWORK_E_H
