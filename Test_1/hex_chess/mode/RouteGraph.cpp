#include "RouteGraph.h"
#include <QSet>

RouteGraph::RouteGraph()
{
}

RouteGraph::~RouteGraph()
{
}

int RouteGraph::Push(const int &row, const int &col)
{
    node.push_back(new RouteNode(row, col, nullptr));
    return node.count() - 1;
}


int RouteGraph::PushBack(const int &row, const int &col, const int &pre_index)
{
    Q_ASSERT(pre_index < node.count());
    node.push_back(new RouteNode(row, col, node[pre_index]));
    return node.count() - 1;
}

int RouteGraph::PushWinner(const int &row, const int &col, const int &pre_index)
{
    Q_ASSERT(pre_index < node.count());
    winner.push_back(RouteNode(row, col, node[pre_index]->PreNode()));
    return winner.count() - 1;
}

QVector<HexLocation> RouteGraph::WinnerRoute_shortest()
{
    Q_ASSERT(winner.count() > 0);
    QSet<QPair<int, int>> shortest;
    RouteNode *p = &winner[0];
    for (int i = 0; i < winner.count(); i++)
    {
        if (p->Depth() > winner[i].Depth())
        {
            p = &winner[i];
        }
    }
    do
    {
        shortest.insert({p->Row(), p->Col()});
        p = p->PreNode();
    }
    while (p != nullptr);
    return QVector<HexLocation>(shortest.begin(), shortest.end());
}

QVector<HexLocation> RouteGraph::WinnerRoute_all()
{
    Q_ASSERT(winner.count() > 0);
    int count = winner.count();
    QSet<QPair<int, int>> all;
    for (int i = 0; i < count; i++)
    {
        RouteNode *p = &winner[i];
        do
        {
            all.insert({p->Row(), p->Col()});
            p = p->PreNode();
        }
        while (p != nullptr);
    }
    return QVector<HexLocation>(all.begin(), all.end());
}

int RouteGraph::Push(const HexLocation &coord)
{
    return Push(coord.row, coord.col);
}


int RouteGraph::PushBack(const HexLocation &coord, const int &pre_index)
{
    return PushBack(coord.row, coord.col, pre_index);
}

int RouteGraph::PushWinner(const HexLocation &coord, const int &pre_index)
{
    return PushWinner(coord.row, coord.col, pre_index);
}
