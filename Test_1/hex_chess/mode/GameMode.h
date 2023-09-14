#ifndef GAMEMODE_H
#define GAMEMODE_H

#include <QObject>
#include <QVector>
#include "HexMatch.hpp"

class GameMode : public QObject
{
    Q_OBJECT
signals:
    void placeChess(int _row_, int _col_);

protected:
    bool *end;
    HexAttacker attacker;
    HexMatch *match;
    QVector<HexPoint> *winnerRoute;

    GameMode(
        bool *end,
        HexMatch *_match,
        QVector<HexPoint> *_winner,
        QObject *parent = nullptr
    );
    virtual bool Outcome();

public:
    virtual ~GameMode();
    virtual void AIWork1();
    virtual void AIWork2();
    virtual bool IsPlayer() = 0;
    virtual void Determine(HexAttacker _attacker) = 0;
};

#endif // GAMEMODE_H
