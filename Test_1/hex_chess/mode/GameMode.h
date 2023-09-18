#ifndef GAMEMODE_H
#define GAMEMODE_H

#include <QObject>
#include <QVector>
#include "HexMatch.hpp"

class GameMode : public QObject
{
    Q_OBJECT
protected:
    GameMode(
        bool *end,
        HexMatch *_match,
        QVector<HexPoint> *_winner,
        HexAttacker *_attacker,
        QObject *parent = nullptr
    );

    bool *end;
    HexMatch *match;
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

#endif // GAMEMODE_H
