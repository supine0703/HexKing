#ifndef MCTSWORK_HPP
#define MCTSWORK_HPP

#include <QObject>
#include <QRunnable>
#include <QSharedPointer>
#include "HexMatch.hpp"
#include "MctsNode.hpp"

QVector<HexPoint> GetValidMoves(const HexMatch& board);

class MctsWork : public QObject, public QRunnable
{
    Q_OBJECT
public:
    MctsWork(
        const QSharedPointer<MctsNode> &child,
        const HexMatch &match,
        QObject *parent = nullptr
    );
    ~MctsWork();
    void run() override;

private:
    HexAttacker SimulatePlayout();
    void Backpropagate(HexAttacker winner);

    HexMatch board;
    QSharedPointer<MctsNode> node;

signals:
    void Finished();
};

#endif // MCTSWORK_HPP
