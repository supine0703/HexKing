#ifndef HEXMATRIX_HPP
#define HEXMATRIX_HPP

#include "HexCellArray.hpp"
#include "qassert.h"

class HexLocation;

class HexMatrix
{
public:
    HexMatrix(const int& order);

    int Order() const;
    int EmptyNum() const;
    int PiecesNum() const;

//------------------------------------------------------------------------------

    HexCell GetCell(const int& index) const;
    HexCell GetCell(const int& row, const int& col) const;
    HexCell GetCell(const HexLocation& point) const;

    HexCell operator()(const int& row, const int& col) const;
    HexCell operator()(const int& index) const;
    HexCell operator()(const HexLocation& point) const;

//------------------------------------------------------------------------------

    HexMatrix& Placed(const int& index, const HexCell& cell);
    HexMatrix& Placed(const int& row, const int& col, const HexCell& cell);
    HexMatrix& Placed(const HexLocation& point, const HexCell& cell);

    HexMatrix& operator()(const int& index, const HexCell& cell);
    HexMatrix& operator()(const int& row, const int& col, const HexCell& cell);
    HexMatrix& operator()(const HexLocation& point, const HexCell& cell);

//------------------------------------------------------------------------------

    HexMatrix& Placed(const int& index, const HexAttacker& cell);
    HexMatrix& Placed(const int& row, const int& col, const HexAttacker& cell);
    HexMatrix& Placed(const HexLocation& point, const HexAttacker& cell);

    HexMatrix& operator()(const int& index, const HexAttacker& cell);
    HexMatrix& operator()(const int& row, const int& col, const HexAttacker& cell);
    HexMatrix& operator()(const HexLocation& point, const HexAttacker& cell);

//------------------------------------------------------------------------------

    void subPiece() {
        --piecesNum;
        Q_ASSERT(piecesNum >= 0);
    }

private:
    uint8_t order;
    HexCellArray cells;
    uint16_t piecesNum;
};


#endif // HEXMATRIX_HPP
