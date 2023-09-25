#ifndef HEXCELL_HPP
#define HEXCELL_HPP

#include <cstdint>

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
    Empty = 2
};

//------------------------------------------------------------------------------

// HexAttacker {
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

inline constexpr bool operator*(const HexAttacker& attacker)
{
    return static_cast<bool>(attacker);
}
// }

//------------------------------------------------------------------------------

// HexCell {
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
    return cell == HexCell::Empty ?
        cell : static_cast<HexCell>(!static_cast<bool>(cell));
}
// }

//------------------------------------------------------------------------------

// HexCell and HexAttacker {
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
// }

#endif // HEXCELL_HPP
