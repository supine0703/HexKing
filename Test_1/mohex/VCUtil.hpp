#ifndef VCUTIL_H
#define VCUTIL_H

#include "HexBoard.hpp"

//----------------------------------------------------------------------------

/** Utilities on VCs. */
namespace VCUtil
{
/** Returns mustplay for color to move. */
bitset_t GetMustplay(const HexBoard& brd, HexColor color);

/** Returns true if carrier defines a valid bridge to the
        edge. Edge and the other endpoint are stored if it is
        valid. */
bool ValidEdgeBridge(const StoneBoard& brd, const bitset_t& carrier,
                     HexPoint& endpoint, HexPoint& edge);

/** Computes responses to probe.
        For each connected group xy, tries to restore connection xy
        after probe p by playing key of semi not touched by p. Also
        finds flaring moves around p. */
void RespondToProbe(const HexBoard& vcbrd, const HexColor toPlay,
                    const HexPoint probe, bitset_t& responses);
}

//----------------------------------------------------------------------------


#endif // VCUTIL_H
