#ifndef ROUTEGRAPH_H
#define ROUTEGRAPH_H

#include <QVector>
#include "HexPoint.hpp"
#include "RouteNode.hpp"

class RouteGraph
{
public:
    RouteGraph();
    ~RouteGraph();

    int Push(const int &row, const int &col);
    int PushBack(const int &row, const int &col, const int &pre_index);
    int PushWinner(const int &row, const int &col, const int &pre_index);
    QVector<HexPoint> WinnerRoute_shortest();
    QVector<HexPoint> WinnerRoute_all();

    int Push(const HexPoint &coord);
    int PushBack(const HexPoint &coord, const int &pre_index);
    int PushWinner(const HexPoint &coord, const int &pre_index);

private:
    QVector<RouteNode*> node;
    QVector<RouteNode> winner;
};

#endif // ROUTEGRAPH_H
