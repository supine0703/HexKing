#ifndef HEXBOARD_HPP
#define HEXBOARD_HPP

#include "HexCellArray.hpp"
#include "qassert.h"

class HexPoint;

class HexBoard
{
public:
    HexBoard(const int& order);

    int Order() const;
    int EmptyNum() const;
    int PiecesNum() const;

//------------------------------------------------------------------------------

    HexCell GetCell(const int& index) const;
    HexCell GetCell(const int& row, const int& col) const;
    HexCell GetCell(const HexPoint& point) const;

    HexCell operator()(const int& row, const int& col) const;
    HexCell operator()(const int& index) const;
    HexCell operator()(const HexPoint& point) const;

//------------------------------------------------------------------------------

    HexBoard& Placed(const int& index, const HexCell& cell);
    HexBoard& Placed(const int& row, const int& col, const HexCell& cell);
    HexBoard& Placed(const HexPoint& point, const HexCell& cell);

    HexBoard& operator()(const int& index, const HexCell& cell);
    HexBoard& operator()(const int& row, const int& col, const HexCell& cell);
    HexBoard& operator()(const HexPoint& point, const HexCell& cell);

//------------------------------------------------------------------------------

    HexBoard& Placed(const int& index, const HexAttacker& cell);
    HexBoard& Placed(const int& row, const int& col, const HexAttacker& cell);
    HexBoard& Placed(const HexPoint& point, const HexAttacker& cell);

    HexBoard& operator()(const int& index, const HexAttacker& cell);
    HexBoard& operator()(const int& row, const int& col, const HexAttacker& cell);
    HexBoard& operator()(const HexPoint& point, const HexAttacker& cell);

//------------------------------------------------------------------------------

    void subPiece() {
        --piecesNum;
        Q_ASSERT(piecesNum >= 0);
    }

private:
    uint8_t order;
    HexCellArray cells;
    uint16_t piecesNum;
};


#endif // HEXBOARD_HPP
