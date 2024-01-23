#ifndef VCOR_H
#define VCOR_H

#include <vector>
#include "Bitset.hpp"

//----------------------------------------------------------------------------

class CarrierList;

std::vector<bitset_t> VCOr(const CarrierList& semis, const CarrierList& fulls,
                           bitset_t xCapturedSet, bitset_t yCapturedSet);

std::vector<bitset_t> VCOr(CarrierList semis, bitset_t fulls_intersect,
                           bitset_t xCapturedSet, bitset_t yCapturedSet);

//----------------------------------------------------------------------------

#endif // VCOR_H
