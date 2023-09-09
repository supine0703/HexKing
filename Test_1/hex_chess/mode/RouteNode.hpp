#ifndef ROUTENODE_H
#define ROUTENODE_H

#include <type_traits>
#include <cstdint>

class RouteNode
{
public:
    RouteNode(const int &_row, const int &_col, RouteNode *const &_pre);
    ~RouteNode();

    int Row() const;
    int Col() const;
    int Depth() const;
    RouteNode *PreNode() const;

private:
    uint8_t row;
    uint8_t col;
    uint8_t depth;
    RouteNode *pre;
};

inline RouteNode::RouteNode(const int &_row, const int &_col, RouteNode *const &_pre)
    : row(_row)
    , col(_col)
    , depth(_pre != nullptr ? _pre->depth : 0)
    , pre(_pre)
{
}

inline RouteNode::~RouteNode()
{
}

inline int RouteNode::Row() const
{
    return static_cast<int>(row);
}

inline int RouteNode::Col() const
{
    return static_cast<int>(col);
}

inline int RouteNode::Depth() const
{
    return static_cast<int>(depth);
}

inline RouteNode* RouteNode::PreNode() const
{
    return pre;
}

#endif // ROUTENODE_H
