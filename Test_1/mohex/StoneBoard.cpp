#include "StoneBoard.hpp"

#include "BoardUtil.hpp"
#include "qdebug.h"
#include <sstream>

//----------------------------------------------------------------------------

StoneBoard::StoneBoard()
    : m_const(0)
    , m_hash(0, 0)
{
}

StoneBoard::StoneBoard(unsigned size)
    : m_const(&ConstBoard::Get(size))
    , m_hash(size, size)
{
    StartNewGame();
}

StoneBoard::StoneBoard(unsigned width, unsigned height)
    : m_const(&ConstBoard::Get(width, height))
    , m_hash(width, height)
{
    StartNewGame();
}

StoneBoard::StoneBoard(unsigned width, unsigned height, const std::string& str)
    : m_const(&ConstBoard::Get(width, height))
    , m_hash(width, height)
{
    SetPosition(str);
}

StoneBoard::~StoneBoard()
{
}

//----------------------------------------------------------------------------

HexColor StoneBoard::GetColor(HexPoint cell) const
{
    Q_ASSERT(Const().IsValid(cell));
    if (IsBlack(cell)) return BLACK;
    if (IsWhite(cell)) return WHITE;
    return EMPTY;
}

bitset_t StoneBoard::GetLegal() const
{
    bitset_t legal;
    if (IsPlayed(RESIGN))
        return legal;

    legal = ~GetPlayed() & Const().GetCells();
    legal.set(RESIGN);

    // Swap is available only when 4 edges and exactly
    // one cell have been played
    if (GetPlayed().count() == 5)
    {
        Q_ASSERT(!IsPlayed(SWAP_PIECES));
        Q_ASSERT(GetColor(FIRST_TO_PLAY).count() >= 3);
        Q_ASSERT((GetPlayed(FIRST_TO_PLAY)).count() == 3);
        Q_ASSERT(GetColor(!FIRST_TO_PLAY).count() == 2);
        legal.set(SWAP_PIECES);
    }
    Q_ASSERT(Const().IsValid(legal));
    return legal;
}

bool StoneBoard::IsLegal(HexPoint cell) const
{
    Q_ASSERT(Const().IsValid(cell));
    return GetLegal().test(cell);
}

//----------------------------------------------------------------------------

void StoneBoard::AddColor(HexColor color, HexPoint cell)
{
    if (color == EMPTY) {
        m_stones[BLACK].reset(cell);
        m_stones[WHITE].reset(cell);
    } else {
        //m_stones[!color].reset(cell);
        m_stones[color].set(cell);
    }
}

void StoneBoard::AddColor(HexColor color, const bitset_t& b)
{
    if (color == EMPTY) {
        m_stones[BLACK] = m_stones[BLACK] - b;
        m_stones[WHITE] = m_stones[WHITE] - b;
    } else {
        //m_stones[!color] -= b;
        m_stones[color] |= b;
    }
}

// Heuristic: color that is the most present among the neighbours.
// @todo implement a better one, the ideal would be a small neural net.
HexColor StoneBoard::PickColor(HexColor color_tb, HexPoint cell)
{
    int black_supp;
    for (BoardIterator n = m_const->Nbs(cell); n; ++n)
    {
        if (IsBlack(*n))
            ++black_supp;
        if (IsWhite(*n))
            --black_supp;
    }
    if (black_supp > 0)
        return BLACK;
    if (black_supp < 0)
        return WHITE;
    return color_tb;
}

// Heuristic: just use color_tb, to be fast.
HexColor StoneBoard::PickColor(HexColor color_tb, const bitset_t& b)
{
    return color_tb;
}

void StoneBoard::SetColor(HexColor color, const bitset_t& b)
{
    Q_ASSERT(HexColorUtil::isBlackWhite(color));
    Q_ASSERT(Const().IsValid(b));
    m_stones[color] = b;
    Q_ASSERT(IsBlackWhiteDisjoint());
}

void StoneBoard::AddPlayed(const bitset_t& played)
{
    m_played |= played;
    ComputeHash();
}

void StoneBoard::SetPlayed(const bitset_t& played)
{
    m_played = played;
    ComputeHash();
}

//----------------------------------------------------------------------------

void StoneBoard::ComputeHash()
{
    // do not include swap in hash value
    bitset_t mask = m_played & Const().GetLocations();
    m_hash.Compute(m_stones[BLACK] & mask, m_stones[WHITE] & mask);
}

void StoneBoard::StartNewGame()
{
    m_played.reset();
    for (BWIterator it; it; ++it)
    {
        m_stones[*it].reset();
        PlayMove(*it, HexPointUtil::colorEdge1(*it));
        PlayMove(*it, HexPointUtil::colorEdge2(*it));
    }
    ComputeHash();
}

void StoneBoard::PlayMove(HexColor color, HexPoint cell)
{
    Q_ASSERT(HexColorUtil::isBlackWhite(color));
    Q_ASSERT(Const().IsValid(cell));

    m_played.set(cell);
    if (Const().IsLocation(cell))
        m_hash.Update(color, cell);
    AddColor(color, cell);
}

void StoneBoard::UndoMove(HexPoint cell)
{
    Q_ASSERT(Const().IsValid(cell));
    HexColor color = GetColor(cell);
    Q_ASSERT(color != EMPTY);

    m_played.reset(cell);
    if (Const().IsLocation(cell))
        m_hash.Update(color, cell);
    AddColor(EMPTY, cell);
}

//----------------------------------------------------------------------------

void StoneBoard::RotateBoard()
{
    m_played = BoardUtil::Rotate(Const(), m_played);
    for (BWIterator it; it; ++it)
        m_stones[*it] = BoardUtil::Rotate(Const(), m_stones[*it]);
    ComputeHash();
}

bool StoneBoard::IsSelfRotation() const
{
    if (m_stones[BLACK] != BoardUtil::Rotate(Const(), m_stones[BLACK]))
        return false;
    if (m_stones[WHITE] != BoardUtil::Rotate(Const(), m_stones[WHITE]))
        return false;
    return true;
}

void StoneBoard::MirrorBoard()
{
    m_played = BoardUtil::Mirror(Const(), m_played);
    for (BWIterator it; it; ++it)
        m_stones[*it] = BoardUtil::Mirror(Const(), m_stones[*it]);
    ComputeHash();
}

//----------------------------------------------------------------------------

void StoneBoard::SetPosition(const StoneBoard& brd)
{
    StartNewGame();
    SetColor(BLACK, brd.GetBlack());
    SetColor(WHITE, brd.GetWhite());
    SetPlayed(brd.GetPlayed());
}

void StoneBoard::SetPositionOnlyPlayed(const StoneBoard& brd)
{
    StartNewGame();
    SetColor(BLACK, brd.GetBlack() & brd.GetPlayed());
    SetColor(WHITE, brd.GetWhite() & brd.GetPlayed());
    SetPlayed(brd.GetPlayed());
}

void StoneBoard::SetPosition(const std::string& str)
{
    /** @note This depends on the order defined by Interior(). */
    StartNewGame();
    int cell = 0;
    for (int i = 0;
         i < str.size() && cell < Width() * Height();
         ++i)
    {
        int x = cell % Width();
        int y = cell / Width();
        HexPoint p = HexPointUtil::coordsToPoint(x, y);
        if (str[i] == '.')
            cell++;
        else if (str[i] == 'B')
        {
            PlayMove(BLACK, p);
            cell++;
        }
        else if (str[i] == 'W')
        {
            PlayMove(WHITE, p);
            cell++;
        }
        else if (str[i] == 'b')
        {
            AddColor(BLACK, p);
            cell++;
        }
        else if (str[i] == 'w')
        {
            AddColor(WHITE, p);
            cell++;
        }
    }
    ComputeHash();
}

//----------------------------------------------------------------------------

std::string StoneBoard::Write() const
{
    return Write(EMPTY_BITSET);
}

std::string StoneBoard::Write(const bitset_t& b) const
{
    std::ostringstream out;
    out << '\n';
    out << "  " << Hash() << '\n';
    out << "  ";
    for (int i = 0; i < Width(); i++)
        out << (char)('a' + i) << "  ";
    out << '\n';
    for (int i = 0; i < Height(); i++)
    {
        for (int j = 0; j < i; j++)
            out << " ";
        if (i + 1 < 10)
            out << " ";
        out << (i + 1) << "\\";
        for (int j = 0; j < Width(); j++)
        {
            HexPoint p = HexPointUtil::coordsToPoint(j, i);
            if (j)
                out << "  ";
            if (b.test(p))
                out << "*";
            else if (IsBlack(p) && IsPlayed(p))
                out << "B";
            else if (IsBlack(p))
                out << "b";
            else if (IsWhite(p) && IsPlayed(p))
                out << "W";
            else if (IsWhite(p))
                out << "w";
            else
                out << ".";
        }
        out << "\\";
        out << (i + 1);
        out << '\n';
    }
    for (int j = 0; j < Height(); j++)
        out << " ";
    out << "   ";
    for (int i = 0; i < Width(); i++)
        out << (char)('a' + i) << "  ";
    return out.str();
}

bool StoneBoard::IsBlackWhiteDisjoint()
{
    if ((m_stones[BLACK] & m_stones[WHITE]).any())
    {
        for (BWIterator it; it; ++it)
            qWarning() << HexPointUtil::ToString(m_stones[*it]) << '\n';
        return false;
    }
    return true;
}

//----------------------------------------------------------------------------
