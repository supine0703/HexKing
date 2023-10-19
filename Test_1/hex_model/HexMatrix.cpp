#include "HexMatrix.hpp"
#include "HexLocation.hpp"

HexMatrix::HexMatrix(const int& order)
    : order(order)
    , cells(order * order)
    , piecesNum(0)
{
}

int HexMatrix::Order() const
{
    return static_cast<int>(order);
}

int HexMatrix::EmptyNum() const
{
    return static_cast<int>(order) * order - piecesNum;
}

int HexMatrix::PiecesNum() const
{
    return static_cast<int>(piecesNum);
}

//------------------------------------------------------------------------------

HexCell HexMatrix::GetCell(const int& index) const
{
    return cells[index];
}

HexCell HexMatrix::GetCell(const int& row, const int& col) const
{
    return cells[col + row * order];
}

HexCell HexMatrix::GetCell(const HexLocation& point) const
{
    return cells[point.col + point.row * order];
}

HexCell HexMatrix::operator()(const int& index) const
{
    return cells[index];
}

HexCell HexMatrix::operator()(const int& row, const int& col) const
{
    return cells[col + row * order];
}

HexCell HexMatrix::operator()(const HexLocation& point) const
{
    return cells[point.col + point.row * order];
}

//------------------------------------------------------------------------------

HexMatrix& HexMatrix::Placed(const int& index, const HexCell& cell)
{
    cells[index] = cell;
    ++piecesNum;
    return *this;
}

HexMatrix& HexMatrix::Placed(const int& row, const int& col, const HexCell& cell)
{
    cells[col + row * order] = cell;
    ++piecesNum;
    return *this;
}

HexMatrix &HexMatrix::Placed(const HexLocation& point, const HexCell& cell)
{
    cells[point.col + point.row * order] = cell;
    ++piecesNum;
    return *this;
}

HexMatrix& HexMatrix::operator()(const int& index, const HexCell& cell)
{
    cells[index] = cell;
    ++piecesNum;
    return *this;
}

HexMatrix& HexMatrix::operator()(const int& row, const int& col, const HexCell& cell)
{
    cells[col + row * order] = cell;
    ++piecesNum;
    return *this;
}

HexMatrix& HexMatrix::operator()(const HexLocation& point, const HexCell& cell)
{
    cells[point.col + point.row * order] = cell;
    ++piecesNum;
    return *this;
}

//------------------------------------------------------------------------------

HexMatrix& HexMatrix::Placed(const int& index, const HexAttacker& cell)
{
    cells[index] = static_cast<HexCell>(cell);
    ++piecesNum;
    return *this;
}

HexMatrix& HexMatrix::Placed(const int& row, const int& col, const HexAttacker& cell)
{
    cells[col + row * order] = static_cast<HexCell>(cell);
    ++piecesNum;
    return *this;
}

HexMatrix& HexMatrix::Placed(const HexLocation& point, const HexAttacker& cell)
{
    cells[point.col + point.row * order] = static_cast<HexCell>(cell);
    ++piecesNum;
    return *this;
}

HexMatrix& HexMatrix::operator()(const int& index, const HexAttacker& cell)
{
    cells[index] = static_cast<HexCell>(cell);
    ++piecesNum;
    return *this;
}

HexMatrix& HexMatrix::operator()(const int& row, const int& col, const HexAttacker& cell)
{
    cells[col + row * order] = static_cast<HexCell>(cell);
    ++piecesNum;
    return *this;
}

HexMatrix& HexMatrix::operator()(const HexLocation& point, const HexAttacker& cell)
{
    cells[point.col + point.row * order] = static_cast<HexCell>(cell);
    ++piecesNum;
    return *this;
}
//------------------------------------------------------------------------------
