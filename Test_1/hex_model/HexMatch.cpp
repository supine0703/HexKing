#include "HexMatch.hpp"
#include <cstring>
#include <QStack>

HexMatch::HexMatch(const int &order) : order(order), cells(new HexCell[order * order])
{
    memset(cells, static_cast<int8_t>(HexCell::Empty), sizeof(HexCell) * order * order);
}

HexMatch::HexMatch(const HexMatch &other)
    : order(other.order)
    , cells(new HexCell[order * order])
{
    memcpy(this->cells, other.cells, sizeof(HexCell) * order * order);
}

HexMatch::~HexMatch()
{
    delete[] cells;
}

HexMatch& HexMatch::operator=(const HexMatch& other)
{
    this->order = other.order;
    if (cells != nullptr)
    {
        delete[] cells;
    }
    cells = new HexCell[order * order];
    memcpy(this->cells, other.cells, sizeof(HexCell) * order * order);
    return *this;
}

HexCell& HexMatch::operator()(const HexPoint& point)
{
    return cells[point.col + point.row * order];
}

HexCell& HexMatch::operator()(const int& row, const int& col)
{
    return cells[col + row * order];
}

const HexCell& HexMatch::operator()(const HexPoint& point) const
{
    return cells[point.col + point.row * order];
}

const HexCell& HexMatch::operator()(const int& row, const int& col) const
{
    return cells[col + row * order];
}

HexCell& HexMatch::GetCell(const HexPoint& point)
{
    return cells[point.col + point.row * order];
}

HexCell& HexMatch::GetCell(const int& row, const int& col)
{
    return cells[col + row * order];
}

const HexCell& HexMatch::GetCell(const HexPoint& point) const
{
    return cells[point.col + point.row * order];
}

const HexCell &HexMatch::GetCell(const int& row, const int& col) const
{
    return cells[col + row * order];
}

int HexMatch::GetOrder() const
{
    return order;
}

bool HexMatch::WinnerDecided(const HexAttacker& attacker) const
{
    int num[2] { 0, 0 };
    int &r = num[0];
    int &c = num[1];

    QStack<HexPoint> stack;
    QVector<HexPoint> visited;

    for (int &p = num[*(!attacker)]; p < order; p++)
    {
        if (GetCell(r, c) == attacker)
        {
            stack.push({r, c});
        }
    }

    // dfs
    while (!stack.isEmpty())
    {
        HexPoint coord = stack.pop();
        r = coord.row;
        c = coord.col;

        if (visited.contains({r, c}))
        {
            continue;
        }

        visited.push_back({r, c});

        // win
        if (num[*attacker] == order - 1)
        {
            return true;
        }
        // push next possible point
        int r_add = r + 1, r_sub = r - 1, c_add = c + 1, c_sub = c - 1;
        if (r > 0)
        {
            if (GetCell(r_sub, c) == attacker)
            {
                stack.push({r_sub, c});
            }
            if (c < order - 1 && GetCell(r_sub, c_add) == attacker)
            {
                stack.push({r_sub, c_add});
            }
        }
        if (r_add < order)
        {
            if (GetCell(r_add, c) == attacker)
            {
                stack.push({r_add, c});
            }
            if (c > 0 && GetCell(r_add, c_sub) == attacker)
            {
                stack.push({r_add, c_sub});
            }
        }
        if (c > 0 && GetCell(r, c_sub) == attacker)
        {
            stack.push({r, c_sub});
        }
        if (c_add < order && GetCell(r, c_add) == attacker)
        {
            stack.push({r, c_add});
        }
    }
    return false;
}
