#ifndef GAMEEVE_H
#define GAMEEVE_H

#include "GameMode.h"
#include <QThread>


class GameEvE : public GameMode
{
    Q_OBJECT
public:
    GameEvE(
        bool *end,
        HexMatch *_match,
        QVector<HexPoint> *_winner,
        HexAttacker *_attacker,
        bool isWhite,
        QObject *parent = nullptr
        );

    void AIWork1() override;
    void AIWork2() override;
    bool IsPlayer() override;
    void Determine(HexAttacker _attacker) override;

private:
    HexAttacker thisAttacker;
    HexAttacker *nowAttacker;
};
#endif // GAMEEVE_H
