#include "HexMatch.hpp"
#include <QStack>

bool HexMatch::WinnerDecided(const HexAttacker& attacker) const
{
    int order = cells.count();
    int num[2] { 0, 0 };
    int &r = num[0];
    int &c = num[1];

    QStack<HexPoint> stack;
    QVector<HexPoint> visited;

    for (int &p = num[*(!attacker)]; p < order; p++)
    {
        if (cells[r][c] == attacker)
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
        int r_add = r+1, r_sub = r-1, c_add = c+1, c_sub = c-1;
        if (r > 0)
        {
            if (cells[r_sub][c] == attacker)
            {
                stack.push({r_sub, c});
            }
            if (c < order - 1 && cells[r_sub][c_add] == attacker)
            {
                stack.push({r_sub, c_add});
            }
        }
        if (r_add < order)
        {
            if (cells[r_add][c] == attacker)
            {
                stack.push({r_add, c});
            }
            if (c > 0 && cells[r_add][c_sub] == attacker)
            {
                stack.push({r_add, c_sub});
            }
        }
        if (c > 0 && cells[r][c_sub] == attacker)
        {
            stack.push({r, c_sub});
        }
        if (c_add < order && cells[r][c_add] == attacker)
        {
            stack.push({r, c_add});
        }
    }
    return false;
}
