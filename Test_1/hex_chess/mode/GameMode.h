#ifndef GAMEPATTERN_H
#define GAMEPATTERN_H

#include <QObject>
#include <QVector>
#include "HexBoard.hpp"
#include "HexPoint.hpp"

class GameMode : public QObject
{
    Q_OBJECT
protected:
    GameMode(
        bool *end,
        HexBoard *_board,
        QVector<HexPoint> *_winner,
        HexAttacker *_attacker,
        QObject *parent = nullptr
    );

    bool *end;
    HexBoard *board;
    QVector<HexPoint> *winnerRoute;
    HexAttacker *nowAttacker;

public:
    virtual ~GameMode();
    virtual void AIWork();
    virtual void Determine();
    virtual bool IsPlayer() = 0;

private:
    bool Outcome();

signals:
    void placeChess(int _row_, int _col_);
};

#endif // GAMEPATTERN_H
