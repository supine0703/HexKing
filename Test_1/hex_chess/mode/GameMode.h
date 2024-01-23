#ifndef GAMEPATTERN_H
#define GAMEPATTERN_H

#include <QObject>
#include <QVector>
#include "HexMatrix.hpp"
#include "HexLocation.hpp"

class GameMode : public QObject
{
    Q_OBJECT
protected:
    GameMode(
        bool *end,
        HexMatrix *_board,
        QVector<HexLocation> *_winner,
        HexAttacker *_attacker,
        QObject *parent = nullptr
    );

    bool *end;
    HexMatrix *board;
    QVector<HexLocation> *winnerRoute;
    HexAttacker *nowAttacker;

public:
    virtual ~GameMode();
    virtual void AIWork();
    virtual void Determine();
    virtual bool IsPlayer() = 0;
    virtual void Exit() { }

private:
    bool Outcome();

signals:
    void placeChess(int _row_, int _col_);
};

#endif // GAMEPATTERN_H
