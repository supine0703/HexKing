#include "ConstBoard.hpp"

#include "BitsetIterator.hpp"
#include "BoardUtil.hpp"
//----------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------

int DistanceToEdge(const ConstBoard& brd, HexPoint from, HexPoint edge)
{
    Q_ASSERT(HexPointUtil::isEdge(edge));

    // 返回边界之间的绝对距离？
    if (HexPointUtil::isEdge(from))
    {
        if (from == edge)                             return 0; // 同一个点
        if (HexPointUtil::oppositeEdge(from) != edge) return 1; // 相邻边界
        if (edge == NORTH || edge == SOUTH)           return brd.Height(); // 相对边界
        return brd.Width(); // 相对边界
    }

    // 返回内部点到边界的绝对距离
    int r, c;
    HexPointUtil::pointToCoords(from, c, r);
    switch(edge)
    {
    case NORTH: return r + 1;
    case SOUTH: return brd.Height() - r;
    case EAST:  return brd.Width() - c;
    default:    return c + 1;
    }
}

//----------------------------------------------------------------------------

} // namespace

//----------------------------------------------------------------------------

ConstBoard& ConstBoard::Get(int size)
{
    return Get(size, size);
}

ConstBoard& ConstBoard::Get(int width, int height)
{
    // Must use a vector of pointers since a vector of ConstBoards could
    // resize itself and invalidate all pointers into it.
    // FIXME: how to ensure memory is freed nicely?
    static std::vector<ConstBoard*> s_brds;
    for (std::size_t i=0; i<s_brds.size(); ++i) {
        if (s_brds[i]->Width() == width && s_brds[i]->Height() == height)
            return *s_brds[i];
    }
    s_brds.push_back(new ConstBoard(width, height));
    return *s_brds.back();
}

//----------------------------------------------------------------------------

ConstBoard::ConstBoard(int size)
    : m_width(size),
    m_height(size)
{
    Q_ASSERT(1 <= m_width && m_width <= MAX_WIDTH);
    Q_ASSERT(1 <= m_height && m_height <= MAX_HEIGHT);
    Init();
}

ConstBoard::ConstBoard(int width, int height)
    : m_width(width),
    m_height(height)
{
    Q_ASSERT(1 <= m_width && m_width <= MAX_WIDTH);
    Q_ASSERT(1 <= m_height && m_height <= MAX_HEIGHT);
    Init();
}

ConstBoard::~ConstBoard()
{
}

//----------------------------------------------------------------------

bool ConstBoard::Adjacent(HexPoint p1, HexPoint p2) const
{
    for (BoardIterator it(Nbs(p1)); it; ++it)
        if (*it == p2)
            return true;
    return false;
}

int ConstBoard::Distance(HexPoint x, HexPoint y) const
{
    Q_ASSERT(IsValid(x));
    Q_ASSERT(IsValid(y));

    // 边界 返回绝对距离
    if (HexPointUtil::isEdge(y))
        return DistanceToEdge(*this, x, y);
    else if (HexPointUtil::isEdge(x))
        return DistanceToEdge(*this, y, x);

    // 内部点 返回绝对距离（单位：格（step））
    int r1, c1, r2, c2;
    HexPointUtil::pointToCoords(x, r1, c1);
    HexPointUtil::pointToCoords(y, r2, c2);
    int dr = r1 - r2;
    int dc = c1 - c2;

    return (dr*dc >= 0)
               ? std::abs(dr) + std::abs(dc)
               : std::max(std::abs(dr), std::abs(dc));
}

//----------------------------------------------------------------------

bitset_t ConstBoard::Nbs(const bitset_t& b, int radius) const
{
    bitset_t nbs = b;
    for (BitsetIterator p (b); p; ++p)
        for (BoardIterator q = Nbs(*p, radius); q; ++q)
            nbs.set(*q);
    return nbs;
}


//----------------------------------------------------------------------

void ConstBoard::Init()
{
//    LogFine() << "--- ConstBoard"
//              << " (" << Width() << " x " << Height() << ")" << '\n';
    ComputePointList(); // 创建（存储）棋盘上的点
    CreateIterators(); // 初始化索引
    ComputeValid(); // 初始化点集
    ComputeNeighbours(); // 初始化邻居列表
    ComputePointInDir(); // 初始化各个方位点左边
    ComputePatternPoints(); // 初始化模式方向上的点
}

void ConstBoard::ComputePointList()
{
    /** @note There are several pieces of code that rely on the fact
        points are visited in the order (a1,b1,...,a2,b2,...,etc) Do
        not change this order unless you know what you are doing!!. */
    for (int p = FIRST_SPECIAL; p < FIRST_CELL; ++p)
        m_points.push_back(static_cast<HexPoint>(p)); // 特殊点 和 边点

    for (int y = 0; y < Height(); y++) // 内部的点
        for (int x = 0; x < Width(); x++)
            m_points.push_back(HexPointUtil::coordsToPoint(x, y));

    m_points.push_back(INVALID_POINT);
}

void ConstBoard::CreateIterators()
{
    int p = 0;
    while (m_points[p] != FIRST_SPECIAL) p++;
    m_all_index = p; // 0 索引开始，swap

    while (m_points[p] != FIRST_EDGE) p++;
    m_locations_index = p; // 边索引

    while (m_points[p] != FIRST_CELL) p++;
    m_cells_index = p; // 点索引
}

void ConstBoard::ComputeValid()
{
    m_valid.reset(); // 置0
    for (BoardIterator i(AllValid()); i; ++i)
        m_valid.set(*i); // 除了0位 和 结尾， 全为 1

    m_locations.reset();
    for (BoardIterator i(EdgesAndInterior()); i; ++i)
        m_locations.set(*i); // 边点和内部点

    m_cells.reset();
    for (BoardIterator i(Interior()); i; ++i)
        m_cells.set(*i); // 只有内部点
}

void ConstBoard::ComputeNeighbours()
{
    // Try all directions for interior cells
    // 迭代所有内部点
    for (BoardIterator i(Interior()); i; ++i)
    {
        int x, y;
        HexPoint cur = *i;
        HexPointUtil::pointToCoords(cur, x, y);
        // 遍历6个方位
        for (int a = 0; a < NUM_DIRECTIONS; a++)
        {
            int fwd = a;
            int lft = (a + 2) % NUM_DIRECTIONS;
            int x1 = x + HexPointUtil::DeltaX(fwd);
            int y1 = y + HexPointUtil::DeltaY(fwd);
            //
            for (int r = 1; r <= Pattern::MAX_EXTENSION; r++)
            {
                int x2 = x1;
                int y2 = y1;
                //
                for (int t = 0; t < r; t++)
                {
                    HexPoint p = BoardUtil::CoordsToPoint(*this, x2, y2);
                    if (p != INVALID_POINT)  // 不等于 0
                    {
                        // Add p to cur's list and cur to p's list
                        // for each radius in [r, MAX_EXTENSION].
                        for (int v=r; v <= Pattern::MAX_EXTENSION; v++)
                        {
                            bool result = false;
                            for (auto& nb : m_neighbours[cur][v])
                                if (nb == p) result = true;
                            if (!result)
                            {
                                m_neighbours[cur][v].push_back(p);
                                result = false;
                            }
                            for (auto& nb : m_neighbours[p][v])
                                if (nb == cur) result = true;
                            if (!result)
                                m_neighbours[p][v].push_back(cur);
                            /*
                            std::vector<HexPoint>::iterator result;

                            result = find(m_neighbours[cur][v].begin(),
                                          m_neighbours[cur][v].end(), p);
                            // 没有找到 点 p 则 加入数组
                            if (result == m_neighbours[cur][v].end())
                                m_neighbours[cur][v].push_back(p);

                            result = find(m_neighbours[p][v].begin(),
                                          m_neighbours[p][v].end(), cur);
                            if (result == m_neighbours[p][v].end())
                                m_neighbours[p][v].push_back(cur);
                            */
                        }
                    }
                    x2 += HexPointUtil::DeltaX(lft);
                    y2 += HexPointUtil::DeltaY(lft);
                }
                x1 += HexPointUtil::DeltaX(fwd);
                y1 += HexPointUtil::DeltaY(fwd);
            }
        }
    }

    /** Add edges to neighbour lists of neighbouring edges.
        @bug NORTH is now distance 2 from SOUTH, but won't appear in
        the neighbour lists for r >= 2; likewise for EAST/WEST.
    */
    // 迭代边和内部的所有点 -- 将边的左右边加入邻居列表
    for (BoardIterator i(EdgesAndInterior()); i; ++i)
    {
        // 如果不是边
        if (!HexPointUtil::isEdge(*i))
            continue;
        // 如果是边 半径之内
        for (int r = 1; r <= Pattern::MAX_EXTENSION; r++)
        {
            // Edges sharing a corner are distance one apart
            m_neighbours[*i][r].push_back(HexPointUtil::leftEdge(*i));
            m_neighbours[*i][r].push_back(HexPointUtil::rightEdge(*i));
        }
    }
    // Null terminate the lists.
    // 加上终止成员
    for (BoardIterator i(EdgesAndInterior()); i; ++i)
        for (int r = 1; r <= Pattern::MAX_EXTENSION; r++)
            m_neighbours[*i][r].push_back(INVALID_POINT);
}

void ConstBoard::ComputePointInDir()
{
    // 遍历内部所有的点
    for (BoardIterator it(Interior()); it; ++it)
    {
        const HexPoint& p = *it;
        // 遍历6个方位
        for (int i = 0; i < 6; ++i)
        {
            // n是方位上的点
            HexPoint n = BoardUtil::PointInDir(*this, p, (HexDirection)i);
            // n不是边且不是内部的点
            if (!HexPointUtil::isEdge(n) && !HexPointUtil::isInteriorCell(n))
            {
                if (BoardUtil::PointInDir(*this, p, DIR_EAST) == EAST)
                {   // top right obtuse corner
                    m_pointInDir[0][p][i] = NORTH;
                    m_pointInDir[1][p][i] = EAST;
                }
                else
                {   // bottem left obtuse corner
                    m_pointInDir[0][p][i] = SOUTH;
                    m_pointInDir[1][p][i] = WEST;
                }
            }
            // n 是内部的点或者是边 0 是黑 1 是白？
            else
            {
                m_pointInDir[0][p][i] = n;
                m_pointInDir[1][p][i] = n;
            }
        }
    }
}

void ConstBoard::ComputePatternPoints()
{
    const static int dir[] =
        { DIR_NORTH,        // ignored!
            // 标准方位
            DIR_NORTH,
            DIR_NORTH_EAST,
            DIR_WEST,
            DIR_EAST,
            DIR_SOUTH_WEST,
            DIR_SOUTH,

            // 模式1方位
            2,               // relative to i - 6
            4,
            1,
            6,
            3,
            5,
            // 模式2方位 同标准
            1,
            2,
            3,
            4,
            5,
            6
        };
    // #if 0
    //           DIR_NORTH_EAST,  // relative to i - 6
    //           DIR_EAST,
    //           DIR_NORTH,
    //           DIR_SOUTH,
    //           DIR_WEST,
    //           DIR_SOUTH_WEST,

        //           DIR_NORTH,       // relative to i - 12
        //           DIR_NORTH_EAST,
        //           DIR_WEST,
        //           DIR_EAST,
        //           DIR_SOUTH_WEST,
        //           DIR_SOUTH
        //         };
        // #endif

        // 遍历所有内部点 （设定标准模式方向上的点）
        for (BoardIterator it(Interior()); it; ++it)
    {
        const HexPoint& p = *it;
        // 6 个方向上的点 对应存储在模式中
        for (int i = 1; i <= 6; ++i)
        {
            m_patternPoint[0][p][i] = m_pointInDir[0][p][ dir[i] ];
            m_patternPoint[1][p][i] = m_pointInDir[1][p][ dir[i] ];
        }
    }

    // 遍历所有内部点
    for (BoardIterator it(Interior()); it; ++it)
    {
        const HexPoint& p = *it;
        // 6个方向
        for (int i = 1; i <= 6; ++i)
        {
            // 黑白双方
            for (int c = 0; c < 2; ++c)
            {
                // 黑白方 所有点 的6个模式方向上的点
                HexPoint n = m_patternPoint[c][p][i];
                // 如果是边
                if (n < FIRST_CELL) // edge
                    m_patternPoint[c][p][i + 6] = n;
                // 如果是内部点 将模式 1 方向上的点天添加
                else
                {
                    const HexPoint m = m_patternPoint[c][n][ dir[i + 6] ];
                    m_patternPoint[c][p][i + 6] = m;
                }
            }
        }
        // 6个方向
        for (int i = 1; i <= 6; ++i)
        {
            for (int c = 0; c < 2; ++c)
            {
                HexPoint n = m_patternPoint[c][p][i];
                if (n < FIRST_CELL) // edge
                    m_patternPoint[c][p][i + 12] = n;
                // 如果是内部点 将模式 2 方向上的点天添加
                else
                {
                    const HexPoint m = m_patternPoint[c][n][ dir[i + 12] ];
                    m_patternPoint[c][p][i + 12] = m;
                }
            }
        }
    }
}

//----------------------------------------------------------------------------
