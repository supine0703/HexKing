#ifndef HEXPOINT_HPP
#define HEXPOINT_HPP

#include <utility>
#include <cstdint>

class HexPoint
{
public:
    HexPoint(const int& row, const int& col) : row(row), col(col) {}
    HexPoint(const std::pair<int, int>& pair) : row(pair.first), col(pair.second) {}
    inline constexpr bool operator==(const HexPoint& other) const
    {
        return (row == other.row) && (col == other.col);
    }
    inline constexpr bool operator!=(const HexPoint& other) const
    {
        return (row != other.row) || (col != other.col);
    }
    uint8_t row;
    uint8_t col;
};

#endif // HEXPOINT_HPP
