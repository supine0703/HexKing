#include "PMoWork.h"

#include <QElapsedTimer>
#include "Decompositions.hpp"
#include "EndgameUtil.hpp"
#include "HexEnvironment.hpp"
#include "HexEval.hpp"
#include "HexLocation.hpp"
#include "HexLog.h"
#include "HexState.hpp"
#include "Resistance.hpp"
#include <QDebug>

PMoWork::PMoWork(const uint &runTime)
    : en(new HexEnvironment(11, 11))
    , brd(new StoneBoard(11))
    , runTime(runTime)
{
    Decompositions::Initialize();
}

PMoWork::~PMoWork()
{
    delete en;
    delete brd;
}

HexLocation PMoWork::Work(QElapsedTimer* time, const HexMatrix &hm,
                  const HexAttacker &attacker)
{
    usedTime = time;
    en->NewGame(11, 11);
    brd->StartNewGame();
    auto indexToPoint = [](int i) {
        return static_cast<HexPoint>(i+7);
    };

    bitset_t around;
    for (int i = 0, end = 121; i < end; i++)
    {
        if (hm(i) != HexCell::Empty)
        {
            around.set(i+7);
            brd->PlayMove((HexColor)hm(i), indexToPoint(i));
        }
    }

    double score;
    HexBoard& hexbrd = en->SyncBoard(*brd);
    HexColor color = static_cast<HexColor>(attacker);
    HexState state(*brd, color);

    //--------------------------------------------------------------------

    HexPoint move = INVALID_POINT;

    InitSearch(hexbrd, color);
    // Calls brd.ComputeAll(state.ToPlay())

    bitset_t consider;

    move = CheckEndgame(hexbrd, color, consider, score);
    // Have been removed from consider :
    // - the cells not in the vc-mustplay ;
    // - the cells inferior according to ComputeAll() ;
    // - the cells losing by strategy stealing argument ;
    // - the cells symetrical by rotation (if symetrical).
    if (move != INVALID_POINT)
    {
        int r, c;
        HexPointUtil::pointToCoords(move, c, r);
        qDebug() << "Best move:" << HexLocation(r, c).Str() << Qt::endl;
        return HexLocation(r, c);
    }

    qDebug() << "Best move cannot be determined, must search state.";

    PointSequence winningSequence;
    bool win = PerformPreSearch(hexbrd, color, consider, winningSequence);
    qDebug() << "must win? " << win << Qt::endl;
    usedTime = nullptr;
    if (win)
    {
        int r, c;
        HexPointUtil::pointToCoords(winningSequence[0], c, r);
        qDebug() << "win move:" << HexLocation(r, c).Str() << Qt::endl;
        return HexLocation(r, c);
    }
    return HexLocation(100,100);
}

void PMoWork::InitSearch(HexBoard &brd, HexColor color)
{
    // Resign if the game is already over
    //    Groups groups;
    //    GroupBuilder::Build(brd.GetPosition(), groups);
    Q_ASSERT(!brd.GetGroups().IsGameOver());

    StoneBoard original(brd.GetPosition());
    brd.ComputeAll(color,INVALID_POINT); // the fillin is added to brd
    if (brd.GetGroups().IsGameOver())
    {
        // Fillin caused win, remove and re-compute without ice.
        qDebug() << "Fillin caused win! Removing...\n";
        brd.GetPosition().SetPosition(original);
        bool oldUseIce = brd.UseICE();
        brd.SetUseICE(false);
        brd.ComputeAll(color);
        brd.SetUseICE(oldUseIce);
        Q_ASSERT(!brd.GetGroups().IsGameOver());
    }
}

HexPoint PMoWork::CheckEndgame(
    HexBoard &brd, HexColor color, bitset_t &consider, double &score)
{
    if (EndgameUtil::IsDeterminedState(brd, color, score))
    {
        HexPoint move = EndgameUtil::PlayDeterminedState(brd, color);
        if (EndgameUtil::IsWonGame(brd, color))
        {
            int r, c;
            HexPointUtil::pointToCoords(move, c, r);
            hexLog() << "Winning! " << HexLocation(r, c).Str()
                     << (color ? hlg::wdl : hlg::bdl);
        }
        else
        {
            int r, c;
            HexPointUtil::pointToCoords(move, c, r);
            hexLog() << "Opponent has won; playing most blocking move! "
                     << HexLocation(r, c).Str()
                     << (color ? hlg::wdl : hlg::bdl);
        }
        return move;
    }
    else
    {
        consider = EndgameUtil::MovesToConsider(brd, color);
        Q_ASSERT(consider.any());
    }
    score = 0;
    if (consider.count() == 1)
    {
        HexPoint move = static_cast<HexPoint>
            (BitsetUtil::FindSetBit(consider));
        int r, c;
        HexPointUtil::pointToCoords(move, c, r);
        hexLog() << "Mustplay is singleton!\n"
                 << HexLocation(r, c).Str()
                 << (color ? hlg::wdl : hlg::bdl);
        qDebug() << "Mustplay is singleton!";
        return move;
    }
    return INVALID_POINT;
}

void PSortConsiderSet(const bitset_t& consider, const Resistance& resist,
                     std::vector<HexPoint>& moves);
bool PMoWork::PerformPreSearch(HexBoard& brd, HexColor color,
                              bitset_t& consider,
                              PointSequence& winningSequence)
{
    bitset_t losing;
    HexColor other = !color;
    bool foundWin = false;

    Resistance resist;
    resist.Evaluate(brd);
    std::vector<HexPoint> moves;
    PSortConsiderSet(consider, resist, moves);

    for (size_t i = 0; i < moves.size() && !foundWin; ++i)
    {
        if (usedTime->elapsed() > runTime)
        {
            qDebug() << " PreSearch: max time reached "
                     << '(' << i << '/' << moves.size() << ").\n";
            break;
        }

        brd.TryMove(color, moves[i]);
        if (EndgameUtil::IsLostGame(brd, other)) // Check for winning move
        {
            PointSequence seq;
            seq.push_back(moves[i]);
            winningSequence = seq;
            foundWin = true;
        }
        else if (EndgameUtil::IsWonGame(brd, other))
            losing.set(moves[i]);
        brd.UndoMove();
    }

    // Abort if we found a one-move win
    if (foundWin)
    {
        int r, c;
        HexPointUtil::pointToCoords(winningSequence[0], c, r);
        hexLog() << "Winning :"
                 << HexLocation(r, c).Str()
                 << (color ? hlg::wdl : hlg::bdl);
        return true;
    }
    else
    {
        hexLog() << "not Found win" << (color ? hlg::wdl : hlg::bdl);
    }

    // Backing up cannot cause this to happen, right?
    Q_ASSERT(!EndgameUtil::IsDeterminedState(brd, color));
    // Use the backed-up ice info to shrink the moves to consider
    //    if (m_backup_ice_info)
    {
        bitset_t new_consider = EndgameUtil::MovesToConsider(brd, color);
//        qDebug() << "Moves to consider for " << (color ? "white" : "black")
//                 << ":" << brd.Write(new_consider).c_str() << '\n';
        // We cannot intersect with the previous consider as there is a
        // risk that not the same choices were made concerning cycles
        // (among other issues)
        if (new_consider.count() < consider.count())
        {
            consider = new_consider;
//            qDebug() << "$$$$$$ new moves to prune $$$$$$"
//                     << brd.Write(consider).c_str() << '\n';
        }
    }

    // Subtract any losing moves from the set we consider, unless all of them
    // are losing (in which case UCT search will find which one resists the
    // loss well).
    if (losing.any())
    {
        if (BitsetUtil::IsSubsetOf(consider, losing))
            qDebug() << "************************************\n"
                     << " All UCT root children are losing!!\n"
                     << "************************************\n";
        else
        {
            qDebug() << "Removed losing moves: " << brd.Write(losing) << '\n';
            consider = consider - losing;
        }
    }

    Q_ASSERT(consider.any());
    return false;
}

inline void PSortConsiderSet(const bitset_t& consider, const Resistance& resist,
                            std::vector<HexPoint>& moves)
{
    std::vector<std::pair<HexEval, HexPoint> > mvsc;
    for (BitsetIterator it(consider); it; ++it)
        mvsc.push_back(std::make_pair(-resist.Score(*it), *it));
    // 归并排序 --从小到大取负，即从大到小
    stable_sort(mvsc.begin(), mvsc.end());
    moves.clear();
    for (std::size_t i = 0; i < mvsc.size(); ++i)
        moves.push_back(mvsc[i].second);
}
