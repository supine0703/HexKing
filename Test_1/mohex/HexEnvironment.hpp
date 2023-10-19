#ifndef HEXENVIRONMENT_HPP
#define HEXENVIRONMENT_HPP

#include "HexBoard.hpp"

//----------------------------------------------------------------------------

/** Groups a HexBoard, ICEngine, and VCBuilderParam objects,
    which correspond to a set of parameters that can be changed. */
struct HexEnvironment
{
    HexEnvironment(int width, int height);

    void NewGame(int width, int height);

    HexBoard& SyncBoard(const StoneBoard& brd);

    ICEngine ice;

    VCBuilderParam buildParam;

    QScopedPointer<HexBoard> brd;

};

//----------------------------------------------------------------------------


#endif // HEXENVIRONMENT_HPP
