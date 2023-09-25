#ifndef HEXAI_H
#define HEXAI_H

#include "HexBoard.hpp"
#include "HexPoint.hpp"
#include <string>

class HexAI
{
    HexAI(const HexAI&) = delete;
    HexAI& operator=(const HexAI&) = delete;
public:
    HexAI() { }
    virtual ~HexAI() { };
    virtual std::string Name() const = 0;
    virtual HexPoint ChooseMove(const HexBoard& board,  HexAttacker attacker) = 0;
};

#endif // HEXAI_H
