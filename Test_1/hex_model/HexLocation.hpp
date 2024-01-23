#ifndef HEXLOCATION_H
#define HEXLOCATION_H

#include <utility>
#include <cstdint>
#include <QString>

class HexLocation
{
public:
    HexLocation(const int& row, const int& col) : row(row), col(col) {}
    HexLocation(const std::pair<int, int>& pair) : row(pair.first), col(pair.second) {}
    inline constexpr bool operator==(const HexLocation& other) const
    {
        return (row == other.row) && (col == other.col);
    }
    inline constexpr bool operator!=(const HexLocation& other) const
    {
        return (row != other.row) || (col != other.col);
    }
    inline QString Str()
    {
        return QString("(") + static_cast<char>(col+65) + ", " + QString::number(11-row) + ")";
    }
    inline int Int(int order)
    {
        return order * row + col;
    }
    uint8_t row;
    uint8_t col;
};

#endif // HEXLOCATION_H
