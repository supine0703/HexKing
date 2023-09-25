#ifndef HEXCELLARRAY_H
#define HEXCELLARRAY_H

#include "HexArrayT.hpp"
#include "HexCell.hpp"

class HexCellArray : public HexArrayT<HexCell>
{
public:
    HexCellArray(const HexCellArray& other) : HexArrayT<HexCell>(other) { }
    HexCellArray(const std::vector<HexCell>& other) : HexArrayT<HexCell>(other) { }
    HexCellArray(const size_t& size): HexArrayT<HexCell>(size)
    {
        memset(array, static_cast<int8_t>(HexCell::Empty), size);
    }
};

#endif // HEXCELLARRAY_H
