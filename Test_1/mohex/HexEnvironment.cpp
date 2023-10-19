#include "HexEnvironment.hpp"

//----------------------------------------------------------------------------

HexEnvironment::HexEnvironment(int width, int height)
    : brd(0)
{
    brd.reset(new HexBoard(width, height, ice, buildParam));
}

void HexEnvironment::NewGame(int width, int height)
{
    if (brd->GetPosition().Width() != width
        && brd->GetPosition().Height() != height)
    {
        /** @todo Make board resizable? Until then, make sure all
            HexBoard parameters are copied here! */
        bool use_vcs = brd->UseVCs();
        bool use_ice = brd->UseICE();
        bool use_dec = brd->UseDecompositions();
        bool backup  = brd->BackupIceInfo();
        brd.reset(new HexBoard(width, height, ice, buildParam));
        brd->SetUseVCs(use_vcs);
        brd->SetUseICE(use_ice);
        brd->SetUseDecompositions(use_dec);
        brd->SetBackupIceInfo(backup);
    }
    brd->GetPosition().StartNewGame();
}

HexBoard& HexEnvironment::SyncBoard(const StoneBoard& board)
{
    brd->GetPosition().SetPosition(board);
    return *brd.get();
}

//----------------------------------------------------------------------------
