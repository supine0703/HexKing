#ifndef ROUTEGRAPH_H
#define ROUTEGRAPH_H

#include <QVector>
#include "HexLocation.hpp"
#include "RouteNode.hpp"

class RouteGraph
{
public:
    RouteGraph();
    ~RouteGraph();

    int Push(const int &row, const int &col);
    int PushBack(const int &row, const int &col, const int &pre_index);
    int PushWinner(const int &row, const int &col, const int &pre_index);
    QVector<HexLocation> WinnerRoute_shortest();
    QVector<HexLocation> WinnerRoute_all();
    
    int Push(const HexLocation &coord);
    int PushBack(const HexLocation &coord, const int &pre_index);
    int PushWinner(const HexLocation &coord, const int &pre_index);

private:
    QVector<RouteNode*> node;
    QVector<RouteNode> winner;
};

#endif // ROUTEGRAPH_H
