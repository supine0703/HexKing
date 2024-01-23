#include "HexEval.hpp"

//----------------------------------------------------------------------------

bool HexEvalUtil::IsValidEval(HexEval ev)
{
    return (ev >= IMMEDIATE_LOSS && ev <= IMMEDIATE_WIN);
}

bool HexEvalUtil::IsWin(HexEval ev)
{
    Q_ASSERT(IsValidEval(ev));
    return ev >= WIN_THRESHOLD;
}

int HexEvalUtil::PlyToWin(HexEval ev)
{
    Q_ASSERT(IsValidEval(ev));
    return (int)(IMMEDIATE_WIN - ev);
}

bool HexEvalUtil::IsLoss(HexEval ev)
{
    Q_ASSERT(IsValidEval(ev));
    return ev <= LOSS_THRESHOLD;
}

int HexEvalUtil::PlyToLoss(HexEval ev)
{
    Q_ASSERT(IsValidEval(ev));
    return (int)(ev - IMMEDIATE_LOSS);
}

bool HexEvalUtil::IsWinOrLoss(HexEval ev)
{
    return IsWin(ev) || IsLoss(ev);
}

//----------------------------------------------------------------------------

