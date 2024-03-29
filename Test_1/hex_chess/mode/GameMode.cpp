#include "GameMode.h"
#include <QStack>
#include <QQueue>
#include "RouteGraph.h"

GameMode::GameMode(bool *end, HexMatrix *_board, QVector<HexLocation> *_winner, HexAttacker *_attacker, QObject *parent)
    : QObject(parent)
    , end(end)
    , board(_board)
    , winnerRoute(_winner)
    , nowAttacker(_attacker)
{
}

GameMode::~GameMode()
{
    end = nullptr;
    board = nullptr;
    winnerRoute = nullptr;
    nowAttacker = nullptr;
}

void GameMode::AIWork()
{
}

void GameMode::Determine()
{
    if (*end)
    {
        return;
    }

    if (Outcome())
    {
        *end = true;
        return;
    }
}

bool GameMode::Outcome()
{
    Q_ASSERT(end != nullptr && board != nullptr && winnerRoute != nullptr);
    HexMatrix &_board = *board;
    int order = _board.Order();
    if (_board.PiecesNum() <= (order - 1) << 1)
    {
        return false;
    }
    int num[2] { 0, 0 };
    int &r = num[0];
    int &c = num[1];
    int depth_i = 0;

    QQueue<QPair<QPair<int, int>, int>> queue;
    QVector<QPair<int, int>> visited;
    QVector<QPair<int, int>> visiting;
    RouteGraph route;

    for (int &p = num[*(!(*nowAttacker))]; p < order; p++)
    {
        if (_board(r, c) == *nowAttacker)
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
        if (num[**nowAttacker] == order - 1)
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
            if (!visited.contains({_r, _c}) && _board(_r, _c) == *nowAttacker)
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
