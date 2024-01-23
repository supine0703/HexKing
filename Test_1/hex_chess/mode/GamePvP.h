#ifndef GAMEPVP_H
#define GAMEPVP_H

#include "GameMode.h"

class GamePvP : public GameMode
{
public:
    GamePvP(
        bool *end,
        HexMatrix *_match,
        QVector<HexLocation> *_winner,
        HexAttacker *_attacker,
        QObject *parent = nullptr
    );

    bool IsPlayer() override;
};

#endif // GAMEPVP_H
