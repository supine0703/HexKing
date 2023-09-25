#include "HexBoard.hpp"
#include "HexPoint.hpp"

HexBoard::HexBoard(const int& order)
    : order(order)
    , cells(order * order)
    , piecesNum(0)
{
}

int HexBoard::Order() const
{
    return static_cast<int>(order);
}

int HexBoard::EmptyNum() const
{
    return static_cast<int>(order) * order - piecesNum;
}

int HexBoard::PiecesNum() const
{
    return static_cast<int>(piecesNum);
}

//------------------------------------------------------------------------------

HexCell HexBoard::GetCell(const int& index) const
{
    return cells[index];
}

HexCell HexBoard::GetCell(const int& row, const int& col) const
{
    return cells[col + row * order];
}

HexCell HexBoard::GetCell(const HexPoint& point) const
{
    return cells[point.col + point.row * order];
}

HexCell HexBoard::operator()(const int& index) const
{
    return cells[index];
}

HexCell HexBoard::operator()(const int& row, const int& col) const
{
    return cells[col + row * order];
}

HexCell HexBoard::operator()(const HexPoint& point) const
{
    return cells[point.col + point.row * order];
}

//------------------------------------------------------------------------------

HexBoard& HexBoard::Placed(const int& index, const HexCell& cell)
{
    cells[index] = cell;
    ++piecesNum;
    return *this;
}

HexBoard& HexBoard::Placed(const int& row, const int& col, const HexCell& cell)
{
    cells[col + row * order] = cell;
    ++piecesNum;
    return *this;
}

HexBoard &HexBoard::Placed(const HexPoint& point, const HexCell& cell)
{
    cells[point.col + point.row * order] = cell;
    ++piecesNum;
    return *this;
}

HexBoard& HexBoard::operator()(const int& index, const HexCell& cell)
{
    cells[index] = cell;
    ++piecesNum;
    return *this;
}

HexBoard& HexBoard::operator()(const int& row, const int& col, const HexCell& cell)
{
    cells[col + row * order] = cell;
    ++piecesNum;
    return *this;
}

HexBoard& HexBoard::operator()(const HexPoint& point, const HexCell& cell)
{
    cells[point.col + point.row * order] = cell;
    ++piecesNum;
    return *this;
}

//------------------------------------------------------------------------------

HexBoard& HexBoard::Placed(const int& index, const HexAttacker& cell)
{
    cells[index] = static_cast<HexCell>(cell);
    ++piecesNum;
    return *this;
}

HexBoard& HexBoard::Placed(const int& row, const int& col, const HexAttacker& cell)
{
    cells[col + row * order] = static_cast<HexCell>(cell);
    ++piecesNum;
    return *this;
}

HexBoard& HexBoard::Placed(const HexPoint& point, const HexAttacker& cell)
{
    cells[point.col + point.row * order] = static_cast<HexCell>(cell);
    ++piecesNum;
    return *this;
}

HexBoard& HexBoard::operator()(const int& index, const HexAttacker& cell)
{
    cells[index] = static_cast<HexCell>(cell);
    ++piecesNum;
    return *this;
}

HexBoard& HexBoard::operator()(const int& row, const int& col, const HexAttacker& cell)
{
    cells[col + row * order] = static_cast<HexCell>(cell);
    ++piecesNum;
    return *this;
}

HexBoard& HexBoard::operator()(const HexPoint& point, const HexAttacker& cell)
{
    cells[point.col + point.row * order] = static_cast<HexCell>(cell);
    ++piecesNum;
    return *this;
}
//------------------------------------------------------------------------------
