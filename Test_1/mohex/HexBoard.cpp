#include "HexBoard.hpp"

#include "BoardUtil.hpp"
#include "Decompositions.hpp"
#include "qdebug.h"
#include <chrono>

//----------------------------------------------------------------------------

HexBoard::HexBoard(int width, int height, const ICEngine& ice,
                   VCBuilderParam& param)
    : m_brd(width, height),
    m_ice(&ice),
    m_groups(),
    m_patterns(m_brd),
    m_use_vcs(true),
    m_use_ice(true),
    m_use_decompositions(true),
    m_backup_ice_info(true),
    m_builder_param(param)
{
    GroupBuilder::Build(m_brd, m_groups);
    for (BWIterator c; c; ++c)
        m_cons[*c].reset(new VCS(*c));
    ClearHistory();
}

/** @warning This is not very maintainable! How to make this
    copy-constructable nicely, even though it has a scoped_ptr? */
HexBoard::HexBoard(const HexBoard& other)
    : m_brd(other.m_brd),
    m_ice(other.m_ice),
    m_groups(other.m_groups),
    m_patterns(m_brd),
    m_history(other.m_history),
    m_inf(other.m_inf),
    m_use_vcs(other.m_use_vcs),
    m_use_ice(other.m_use_ice),
    m_use_decompositions(other.m_use_decompositions),
    m_backup_ice_info(other.m_backup_ice_info),
    m_builder_param(other.m_builder_param)
{
    m_patterns.CopyState(other.GetPatternState());
    for (BWIterator c; c; ++c)
    {
        m_cons[*c].reset(new VCS(*c));
    }
}

HexBoard::~HexBoard()
{
}

//----------------------------------------------------------------------------

HexPoint HexBoard::ComputeInferiorCells(HexColor color_to_move,
                                        HexPoint last_move,
                                        bool only_around_last_move)
{
    if (m_use_ice)
    {
        InferiorCells inf;
        HexPoint reverser =
            m_ice->ComputeInferiorCells(color_to_move, m_groups,
                                        m_patterns, inf, last_move,
                                        only_around_last_move);
        IceUtil::Update(m_inf, inf);
        return reverser;
    }
    return INVALID_POINT;
}

void HexBoard::BuildVCs()
{
    for (BWIterator c; c; ++c)
        m_cons[*c]->Build(m_builder_param, m_groups, m_patterns);
}

void HexBoard::BuildVCs(const Groups& oldGroups,
                        bitset_t added[BLACK_AND_WHITE], bool use_changelog)
{
    Q_ASSERT((added[BLACK] & added[WHITE]).none());
    for (BWIterator c; c; ++c)
        m_cons[*c]->Build(m_builder_param, oldGroups, m_groups,
                          m_patterns, added, use_changelog);
}

void HexBoard::RevertVCs()
{
    for (BWIterator c; c; ++c)
        m_cons[*c]->Revert();
}

/** In non-terminal states, checks for combinatorial decomposition
    with a vc using Decompositions::Find(). Plays the carrier using
    AddStones(). Loops until no more decompositions are found. */
void HexBoard::HandleVCDecomposition(HexColor color_to_move)
{
    if (!m_use_decompositions)
        return;

    /** @todo Check for a vc win/loss here instead of just solid
    chains. */
    if (m_groups.IsGameOver())
        return;

    int decompositions = 0;
    for (;;)
    {
        bool found = false;
        for (BWIterator c; c; ++c)
        {
            bitset_t fillin;
            if (Decompositions::Find(*this, *c, fillin))
            {
                //qDebug() << "Decomposition " << decompositions << ": for " << *c << ".\n" << m_brd.Write(fillin) << '\n';

                AddStones(*c, fillin, color_to_move);
                m_inf.AddFillin(*c, fillin);

                //qDebug() << "After decomposition " << decompositions << ": " << (std::ostringstream() << m_brd).str() << '\n';

                decompositions++;
                found = true;
                break;
            }
        }
        if (!found)
            break;
    }
    //qDebug() << "Found " << decompositions << " decompositions.\n";
}

HexPoint HexBoard::ComputeAll(HexColor color_to_move, HexPoint last_move,
                              bool add_fillin, bool only_around_last_move)
{
    auto start = std::chrono::steady_clock::now();

    m_patterns.Update();
    GroupBuilder::Build(m_brd, m_groups);
    m_inf.Clear();

    Groups old_groups(m_groups);
    HexPoint reverser =
        ComputeInferiorCells(color_to_move, last_move, only_around_last_move);

    if (m_use_vcs)
    {
        BuildVCs();
        HandleVCDecomposition(color_to_move);
    }
    auto end = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    //qDebug() << time.count() << "ms to compute all.\n";

    if (add_fillin)
    {
        m_brd.AddColor(BLACK, m_inf.Fillin(BLACK));
        m_brd.AddColor(WHITE, m_inf.Fillin(WHITE));
        m_brd.AddPlayed(m_inf.Fillin(BLACK) | m_inf.Fillin(WHITE));

        bitset_t added[BLACK_AND_WHITE];
        added[BLACK] = m_inf.Fillin(BLACK);
        added[WHITE] = m_inf.Fillin(WHITE);
        if (m_use_vcs)
            BuildVCs(old_groups, added, false);

        m_patterns.Update(added[BLACK] | added[WHITE]);
    }

    if (m_inf.Fillin(BLACK).test(reverser) || m_inf.Fillin(WHITE).test(reverser))
        reverser = INVALID_POINT;
    return reverser;
}

void HexBoard::PlayMove(HexColor color, HexPoint cell)
{
    //qDebug() << "Playing (" << color << ", " << cell << ")\n";

    auto start = std::chrono::steady_clock::now();
    PushHistory(color, cell);
    bitset_t old_black = m_brd.GetColor(BLACK);
    bitset_t old_white = m_brd.GetColor(WHITE);

    m_brd.PlayMove(color, cell);
    m_patterns.Update(cell);
    Groups oldGroups(m_groups);
    GroupBuilder::Build(m_brd, m_groups);

    ComputeInferiorCells(!color);

    bitset_t added[BLACK_AND_WHITE];
    added[BLACK] = m_brd.GetColor(BLACK) - old_black;
    added[WHITE] = m_brd.GetColor(WHITE) - old_white;

    if (m_use_vcs)
    {
        BuildVCs(oldGroups, added, true);
        HandleVCDecomposition(!color);
    }
    auto end = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    //qDebug() << time.count() << "ms to play stones.\n";
}

void HexBoard::TryMove(HexColor color, HexPoint cell)
{
    //qDebug() << "Playing (" << color << ", " << cell << ")\n";

    auto start = std::chrono::steady_clock::now();
    PushHistory(color, cell);
    bitset_t old = m_brd.GetColor(color);

    m_brd.PlayMove(color, cell);
    m_patterns.Update(cell);
    Groups oldGroups(m_groups);
    GroupBuilder::Build(m_brd, m_groups);

    if (m_use_ice)
    {
        InferiorCells inf;
        m_ice->ComputeFillin(m_groups, m_patterns, inf, color,
                             ICEngine::MONOCOLOR_USING_CAPTURED);
        IceUtil::Update(m_inf, inf);
    }

    if (m_use_vcs)
    {
        bitset_t added[BLACK_AND_WHITE];
        added[color] = m_brd.GetColor(color) - old;
        BuildVCs(oldGroups, added, true);
        HandleVCDecomposition(!color);
    }
    auto end = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    //qDebug() << time.count() << "ms to try move.\n";
}

void HexBoard::PlayStones(HexColor color, const bitset_t& played,
                          HexColor color_to_move)
{
    //qDebug() << "Playing (" << color << "," << HexPointUtil::ToString(played) << ")\n";
    Q_ASSERT(BitsetUtil::IsSubsetOf(played, GetPosition().GetEmpty()));

    auto start = std::chrono::steady_clock::now();
    PushHistory(color, INVALID_POINT);
    bitset_t old_black = m_brd.GetColor(BLACK);
    bitset_t old_white = m_brd.GetColor(WHITE);

    m_brd.AddColor(color, played);
    m_patterns.Update(played);
    Groups oldGroups(m_groups);
    GroupBuilder::Build(m_brd, m_groups);

    ComputeInferiorCells(color_to_move);

    bitset_t added[BLACK_AND_WHITE];
    added[BLACK] = m_brd.GetColor(BLACK) - old_black;
    added[WHITE] = m_brd.GetColor(WHITE) - old_white;

    if (m_use_vcs)
    {
        BuildVCs(oldGroups, added, true);
        HandleVCDecomposition(color_to_move);
    }

    auto end = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    //qDebug() << time.count() << "ms to play stones.\n";
}

/** Adds stones for color to board with color_to_move about to
    play next; added stones must be a subset of the empty cells.
    Does not affect the hash of this state. State is not pushed
    onto stack, so a call to UndoMove() will undo these changes
    along with the last changes that changed the stack. */
void HexBoard::AddStones(HexColor color, const bitset_t& played,
                         HexColor color_to_move)
{
    Q_ASSERT(BitsetUtil::IsSubsetOf(played, GetPosition().GetEmpty()));
    //qDebug() << "Adding (" << color << ", " << HexPointUtil::ToString(played) << ")\n";

    auto start = std::chrono::steady_clock::now();
    bitset_t old_black = m_brd.GetColor(BLACK);
    bitset_t old_white = m_brd.GetColor(WHITE);

    m_brd.AddColor(color, played);
    m_patterns.Update(played);
    Groups oldGroups(m_groups);
    GroupBuilder::Build(m_brd, m_groups);

    ComputeInferiorCells(color_to_move);

    bitset_t added[BLACK_AND_WHITE];
    added[BLACK] = m_brd.GetColor(BLACK) - old_black;
    added[WHITE] = m_brd.GetColor(WHITE) - old_white;

    if (m_use_vcs)
        BuildVCs(oldGroups, added, false);

    auto end = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    //qDebug() << time.count() << "ms to add stones.\n";
}

void HexBoard::UndoMove()
{
    auto start = std::chrono::steady_clock::now();
    PopHistory();
    m_patterns.Update();
    auto end = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    //qDebug() << time.count() << "ms to undo move.\n";
}


//----------------------------------------------------------------------------

void HexBoard::ClearHistory()
{
    m_history.clear();
}

void HexBoard::PushHistory(HexColor color, HexPoint cell)
{
    m_history.push_back(History(m_brd, m_groups, m_inf, color, cell));
}

/** Restores the old board position, backs up ice info, and reverts
    virtual connections. */
void HexBoard::PopHistory()
{
    Q_ASSERT(!m_history.empty());

    History hist = m_history.back();
    m_history.pop_back();

    m_brd.SetPosition(hist.board);
    if (m_backup_ice_info && hist.last_played != INVALID_POINT)
    {
        // Cells that were not marked as inferior in parent state
        // and are monocolor-filled are marked as dominated.
        // @warning : this may give wrong pruning if the fill was
        // not monocolor
        bitset_t a = m_brd.GetEmpty() - hist.inf.All();
        a &= m_inf.Fillin(hist.to_play);

        for (BitsetIterator p(a); p; ++p)
            hist.inf.AddInferior(*p, hist.last_played);

        if (m_brd.IsSelfRotation())
        {
            // In this case, the computation is done on only half of the
            // board, and we want EndgameUtil::RemoveRotation to work
            // properly, which forces to add the symetrical inferior.
            HexPoint rot_last_played =
                BoardUtil::Rotate(m_brd.Const(), hist.last_played);
            for (BitsetIterator p(a); p; ++p)
                hist.inf.AddInferior
                    (BoardUtil::Rotate(m_brd.Const(), *p), rot_last_played);
        }

    }
    m_inf = hist.inf;
    m_groups = hist.groups;
    RevertVCs();
}

//----------------------------------------------------------------------------

