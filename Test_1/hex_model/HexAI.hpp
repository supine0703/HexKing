#ifndef HEXAI_H
#define HEXAI_H

#include "HexMatch.hpp"

class QString;

class HexAI
{
public:
    virtual ~HexAI() { };
    virtual QString Name() = 0;
    virtual HexPoint ChooseMove(const HexMatch& board,  HexAttacker attacker) = 0;
};

#endif // HEXAI_H
