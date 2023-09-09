#ifndef HEXATTACKER_HPP
#define HEXATTACKER_HPP

#include <QPair>
#include <QVector>

using HexAttacker_t = bool;
enum class HexAttacker : HexAttacker_t
{
    Black = 0,
    White = 1
};

using HexCell_t = int8_t;
enum class HexCell : HexCell_t
{
    Black = 0,
    White = 1,
    Empty = -1
};

class HexPoint
{
public:
    HexPoint(const int& row, const int& col) : row(row), col(col) {}
    HexPoint(const QPair<int, int>& pair) : row(pair.first), col(pair.second) {}
    inline constexpr bool operator==(const HexPoint& other) const
    {
        return row == other.row && col == other.col;
    }
    inline constexpr bool operator!=(const HexPoint& other) const
    {
        return row != other.row || col != other.col;
    }
    uint8_t row;
    uint8_t col;
};

class HexMatch
{
public:
    HexMatch(const int& order) : order(order), cells(order, QVector<HexCell>(order, HexCell::Empty)) {}
    QVector<HexCell>& operator[](const int& i) { return cells[i]; }
    HexCell& GetCell(const HexPoint& coord) { return cells[coord.row][coord.col]; }
    HexCell& GetCell(const int& row, const int& col) { return cells[row][col]; }

    const QVector<HexCell>& operator[](const int& i) const { return cells[i]; }
    const HexCell& GetCell(const HexPoint& coord) const { return cells[coord.row][coord.col]; }
    const HexCell& GetCell(const int& row, const int& col) const { return cells[row][col]; }

    auto begin() const { return cells.begin(); }
    auto end() const { return cells.end(); }
    auto begin() { return cells.begin(); }
    auto end() { return cells.end(); }

    int Order() const { return order; }
    bool WinnerDecided(const HexAttacker& attacker) const;

private:
    const uint8_t order;
    QVector<QVector<HexCell>> cells;
};

//-------------------------HexAttacker------------------------
inline constexpr bool operator==(const HexAttacker& a, const bool& b)
{
    return static_cast<bool>(a) == b;
}

inline constexpr bool operator==(const bool& a, const HexAttacker& b)
{
    return static_cast<bool>(b) == a;
}

inline constexpr bool operator!=(const HexAttacker& a, const bool& b)
{
    return static_cast<bool>(a) != b;
}

inline constexpr bool operator!=(const bool& a, const HexAttacker& b)
{
    return static_cast<bool>(b) != a;
}

inline constexpr HexAttacker operator!(const HexAttacker& attacker)
{
    return static_cast<HexAttacker>(!static_cast<bool>(attacker));
}
//-------------------------HexAttacker------------------------

//----------------------------Cell----------------------------
inline constexpr bool operator==(const HexCell& a, const bool& b)
{
    return a == HexCell::Empty ? false : static_cast<bool>(a) == b;
}

inline constexpr bool operator==(const bool& a, const HexCell& b)
{
    return b == HexCell::Empty ? false : static_cast<bool>(b) == a;
}

inline constexpr bool operator!=(const HexCell& a, const bool& b)
{
    return a == HexCell::Empty ? true : static_cast<bool>(a) != b;
}

inline constexpr bool operator!=(const bool& a, const HexCell& b)
{
    return b == HexCell::Empty ? true : static_cast<bool>(b) != a;
}

inline constexpr HexCell operator!(const HexCell& cell)
{
    return cell == HexCell::Empty ? cell : static_cast<HexCell>(!static_cast<bool>(cell));
}
//----------------------------Cell----------------------------

//--------------------Cell and HexAttacker--------------------
inline constexpr bool operator==(const HexCell& a, const HexAttacker& b)
{
    return a == static_cast<bool>(b);
}

inline constexpr bool operator==(const HexAttacker& a, const HexCell& b)
{
    return static_cast<bool>(a) == b;
}

inline constexpr bool operator!=(const HexCell& a, const HexAttacker& b)
{
    return a != static_cast<bool>(b);
}

inline constexpr bool operator!=(const HexAttacker& a, const HexCell& b)
{
    return static_cast<bool>(a) != b;
}
//--------------------Cell and HexAttacker--------------------

#define _BLACK_ QColor(0, 0, 0, 255)
#define _WHITE_ QColor(255, 255, 255, 255)
#define _RED_ QColor(234, 67, 53, 255)
#define _BLUE_ QColor(66, 133, 244, 255)
#define _GRAY_ QColor(191, 191, 191, 255)
#define _YELLOW_ QColor(255, 225, 135, 255)

#define _RED_T_ QColor(234, 67, 53, 127)
#define _BLUE_T_ QColor(66, 133, 244, 127)

#endif // HEXATTACKER_HPP
