#ifndef HEXAI_H
#define HEXAI_H

#include "HexMatch.hpp"

class QString;

class HexAI
{
protected:
    HexAI(const QString& name);

    QString *name;

public:
    virtual ~HexAI();
    virtual QString GetName();
    virtual HexPoint ChooseMove(const HexMatch& board,  HexAttacker attacker) = 0;
};

#endif // HEXAI_H
