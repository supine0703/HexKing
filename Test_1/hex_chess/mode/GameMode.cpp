#include "GameMode.h"
#include <QStack>
#include <QQueue>
#include "RouteGraph.h"

GameMode::GameMode(bool *end, HexMatch *_match, QVector<HexPoint> *_winner, QObject *parent)
    : QObject(parent)
    , end(end)
    , match(_match)
    , winnerRoute(_winner)
{
}

GameMode::~GameMode()
{
    end = nullptr;
    match = nullptr;
    winnerRoute = nullptr;
}

void GameMode::AIWork()
{
}

bool GameMode::Outcome()
{
    Q_ASSERT(end != nullptr && match != nullptr && winnerRoute != nullptr);
    HexMatch &_match = *match;
    if (!match->WinnerDecided(attacker))
    {
        return false;
    }
    int order = _match.GetOrder();
    int num[2] { 0, 0 };
    int &r = num[0];
    int &c = num[1];
    int depth_i = 0;

    QQueue<QPair<QPair<int, int>, int>> queue;
    QVector<QPair<int, int>> visited;
    QVector<QPair<int, int>> visiting;
    RouteGraph route;

    for (int &p = num[*(!attacker)]; p < order; p++)
    {
        if (_match(r, c) == attacker)
        {
            depth_i++;
            queue.enqueue({{r, c}, route.Push(r, c)});
        }
    }

    // BFS
    while (!queue.isEmpty())
    {
        auto [coord, index] = queue.dequeue();
        r = coord.first;
        c = coord.second;

        if (index == depth_i)
        {
            depth_i += visiting.count();
            visited.append(visiting);
            visiting.clear();
        }

        visiting.push_back({r, c});

        // win
        if (num[*attacker] == order - 1)
        {
            route.PushWinner(r, c, index);
            *end = true;
            continue;
        }

        // push next possible point
        auto _enQ = [&](int r_add, int c_add, int _i)
        {
            // return table
            switch (r_add * 3 + c_add)
            {
            case -2: // r-1, c+1
                if (c >= order - 1)
                {
                    return;
                }
            case -3: // r-1, c
                if (r < 1)
                {
                    return;
                }
                break;
            case -1: // r, c-1
                if (c < 1)
                {
                    return;
                }
                break;
            case 2: // r+1, c-1
                if (c < 1)
                {
                    return;
                }
            case 3: // r+1, c
                if (r >= order - 1)
                {
                    return;
                }
                break;
            case 1: // r, c+1
                if (c >= order - 1)
                {
                    return;
                }
            }
            int _r = r + r_add;
            int _c = c + c_add;
            if (!visited.contains({_r, _c}) && _match(_r, _c) == attacker)
            {
                queue.enqueue({{_r, _c}, route.PushBack(_r, _c, _i)});
            }
        };
        _enQ(1, 0, index);
        _enQ(0, 1, index);
        _enQ(-1, 1, index);
        _enQ(-1, 0, index);
        _enQ(0, -1, index);
        _enQ(1, -1, index);
    }
    if (*end)
    {
        *winnerRoute = route.WinnerRoute_shortest();
//        *winnerRoute = route.WinnerRoute_all();
    }
    return *end;
}
