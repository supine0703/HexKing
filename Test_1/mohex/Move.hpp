#ifndef MOVE_HPP
#define MOVE_HPP

#include "HexColor.hpp"
#include "HexPoint.hpp"
#include <sstream>

//----------------------------------------------------------------------------

/** A (HexColor, HexPoint) pair. */
class Move
{
public:

    /** Creates the move (color, point). */
    Move(HexColor color, HexPoint point);

    /** Returns color of move. */
    HexColor Color() const;

    /** Returns point of move. */
    HexPoint Point() const;

    /** Outputs a [color, move] string. */
    std::string ToString() const;

    /** Returns true if point and color are equal. */
    bool operator==(const Move& other) const;

    /** Returns true !operator==(other). */
    bool operator!=(const Move& other) const;

private:
    HexColor m_color;

    HexPoint m_point;
};

inline Move::Move(HexColor color, HexPoint point)
    : m_color(color),
    m_point(point)
{
}

inline HexColor Move::Color() const
{
    return m_color;
}

inline HexPoint Move::Point() const
{
    return m_point;
}

inline std::string Move::ToString() const
{
    std::ostringstream os;
    os << '[' << m_color << ", " << m_point << ']';
    return os.str();
}

inline bool Move::operator==(const Move& other) const
{
    return m_color == other.m_color
           && m_point == other.m_point;
}

inline bool Move::operator!=(const Move& other) const
{
    return !operator==(other);
}

//----------------------------------------------------------------------------

/** Extends standard output operator to Moves. */
inline std::ostream& operator<<(std::ostream& os, const Move& move)
{
    return os << move.ToString();
}

//----------------------------------------------------------------------------

/** Sequence of moves. */
typedef std::vector<Move> MoveSequence;

/** Extends standard output operator for MoveSequences. */
inline std::ostream& operator<<(std::ostream& os, const MoveSequence& move)
{
    for (std::size_t i = 0; i < move.size(); ++i)
        os << move[i] << ' ';
    return os;
}

namespace MoveSequenceUtil
{
/** Returns INVALID_POINT if history is empty, otherwise last move
        played to the board, ie, skips swap move. */
inline HexPoint LastMoveFromHistory(const MoveSequence& history)
{
    HexPoint lastMove = INVALID_POINT;
    if (!history.empty())
    {
        lastMove = history.back().Point();
        if (lastMove == SWAP_PIECES)
        {
            Q_ASSERT(history.size() == 2);
            lastMove = history.front().Point();
        }
    }
    return lastMove;
}

/** Returns true if one is a prefix of the other. */
inline bool IsPrefixOf(const MoveSequence& a, const MoveSequence& b)
{
    for (std::size_t i = 0; i < a.size() && i < b.size(); ++i)
    {
        if (a[i] != b[i])
            return false;
    }
    return true;
}
}

//----------------------------------------------------------------------------


#endif // MOVE_HPP
