#include "Pattern.hpp"

#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>
#include "Bitset.hpp"
#include "HexPoint.hpp"
#include "SgException.hpp"
#include "qdebug.h"
#include <QException>

//----------------------------------------------------------------------------

Pattern::Pattern()
    : m_type(Pattern::UNKNOWN),
    m_name("unknown"),
    m_comment(""),
    m_flags(0),
    m_weight(0)
{
    memset(m_slice, 0, sizeof(m_slice));
}

//----------------------------------------------------------------------------

std::string Pattern::Serialize() const
{
    std::ostringstream os;
    os << m_type << ":";
    for (int s = 0; s < NUM_SLICES; s++)
    {
        for (int a = 0; a < NUM_FEATURES; a++)
        {
            if (a)
                os << ",";
            os << (int)m_slice[s][a];
        }
        os << ";";
    }
    return os.str();
}

bool Pattern::Unserialize(std::string code)
{
    std::istringstream ss(code);
    char c;
    unsigned int i;
    // mask of all valid bits in a slice
    // 将低5位全设置成1
    const unsigned int check = ~((1<<(MAX_EXTENSION*(MAX_EXTENSION+1)/2))-1);

    ss >> c;
    m_type = c;

    // set the flags based on the type
    m_flags = 0;
    /*switch(m_type)
    {
    case MOHEX:
    case SHIFT:
        m_flags |= HAS_WEIGHT;
        break;
    }*/

    m_extension = 0;
    m_empty.clear();
    m_moves1.clear();
    m_moves2.clear();
    for (int s = 0; s < Pattern::NUM_SLICES; s++)
    {
        int empty = 0;
        for (int j = 0; j < Pattern::NUM_FEATURES; j++)
        {
            ss >> c;                           // skips ':', ';', and ','
            ss >> i;
            if ((i & check) != 0)
                return false;
            m_slice[s][j] = i;
            m_extension = std::max(m_extension,
                                   PatternUtil::GetExtensionFromGodel(i));
            // Computation of the set of empty cells
            if (j == FEATURE_CELLS)
                empty = i;
            if (j == FEATURE_BLACK || j == FEATURE_WHITE)
                empty -= i;
            // Handle moves encoded in first marked set
            if ((i != 0) && (j == FEATURE_MARKED1))
            {
                m_flags |= HAS_MOVES1;
                for (int b = 0; b < 31; b++)
                    if (i & (1U << b))
                        m_moves1.push_back(std::make_pair(s, b));
            }
            // Handle moves encoded in second marked set
            if ((i != 0) && (j == FEATURE_MARKED2))
            {
                m_flags |= HAS_MOVES2;
                for (int b = 0; b < 31; b++)
                    if (i & (1U << b))
                        m_moves2.push_back(std::make_pair(s, b));
            }
        }
        if (empty)
        {
            m_flags |= HAS_EMPTY;
            for (int b = 0; b < 31; b++)
                if (empty & (1U << b))
                    m_empty.push_back(std::make_pair(s, b));
        }
        if (!CheckSliceIsValid(m_slice[s]))
            return false;
    }
    if (m_flags & HAS_WEIGHT)
    {
        ss >> c;                          // skips ':'
        ss >> i;
        m_weight = i;
    }
    ComputeRingGodel();
    return true;
}

bool Pattern::CheckSliceIsValid(const slice_t& slice) const
{
    if (!BitsetUtil::IsSubsetOf(slice[FEATURE_BLACK], slice[FEATURE_CELLS]))
        return false;
    if (!BitsetUtil::IsSubsetOf(slice[FEATURE_WHITE], slice[FEATURE_CELLS]))
        return false;
    if (!BitsetUtil::IsSubsetOf(slice[FEATURE_MARKED1], slice[FEATURE_CELLS]))
        return false;
    if (!BitsetUtil::IsSubsetOf(slice[FEATURE_MARKED2], slice[FEATURE_CELLS]))
        return false;
    if ((slice[FEATURE_BLACK] & slice[FEATURE_WHITE]) != 0)
        return false;
    return true;
}

void Pattern::FlipColors()
{
    for (int s = 0; s < NUM_SLICES; s++)
        std::swap(m_slice[s][Pattern::FEATURE_BLACK],
                  m_slice[s][Pattern::FEATURE_WHITE]);
    ComputeRingGodel();
}

//----------------------------------------------------------------------------

void Pattern::Mirror()
{
    int data[32][32][Pattern::NUM_FEATURES];
    memset(data, 0, sizeof(data));
    // Unpack the pattern into the grid centered at (10,10).
    // flip the x,y coords of each piece and store its information
    for (int s = 0; s < NUM_SLICES; s++)
    {
        int fwd = s;
        int lft = (s + 2) % NUM_DIRECTIONS;
        int x1 = 10 + HexPointUtil::DeltaX(fwd);
        int y1 = 10 + HexPointUtil::DeltaY(fwd);
        for (int i = 1, g = 0; i <= Pattern::MAX_EXTENSION; i++)
        {
            int x2 = x1;
            int y2 = y1;
            for (int j = 0; j < i; j++)
            {
                for (int k = 0; k < Pattern::NUM_FEATURES; k++)
                {
                    // NOTE: (x, y) coords are flipped here to mirror it
                    data[x2][y2][k] = ((1 << g) & m_slice[s][k]);
                }

                x2 += HexPointUtil::DeltaX(lft);
                y2 += HexPointUtil::DeltaY(lft);
                g++;
            }
            x1 += HexPointUtil::DeltaX(fwd);
            y1 += HexPointUtil::DeltaY(fwd);
        }
    }

    // Run over it again and reconstruct the flipped pattern information
    m_empty.clear();
    m_moves1.clear();
    m_moves2.clear();
    memset(m_slice, 0, sizeof(m_slice));
    for (int s = 0; s < NUM_SLICES; s++)
    {
        int fwd = s;
        int lft = (s + 2) % NUM_DIRECTIONS;
        int x1 = 10 + HexPointUtil::DeltaX(fwd);
        int y1 = 10 + HexPointUtil::DeltaY(fwd);
        for (int i = 1, g = 0; i <= Pattern::MAX_EXTENSION; i++)
        {
            int x2 = x1;
            int y2 = y1;
            for (int j = 0; j < i; j++)
            {
                bool empty = false;
                for (int k = 0; k < Pattern::NUM_FEATURES; k++)
                {
                    if (data[y2][x2][k])
                    {
                        m_slice[s][k] ^= (1 << g);
                        if (k == FEATURE_CELLS)
                            empty = true;
                        if (k == FEATURE_BLACK || k == FEATURE_WHITE)
                            empty = false;
                        if (k == FEATURE_MARKED1)
                            m_moves1.push_back(std::make_pair(s, g));
                        if (k == FEATURE_MARKED2)
                            m_moves2.push_back(std::make_pair(s, g));
                    }
                }
                if (empty)
                    m_empty.push_back(std::make_pair(s, g));
                x2 += HexPointUtil::DeltaX(lft);
                y2 += HexPointUtil::DeltaY(lft);
                g++;
            }
            x1 += HexPointUtil::DeltaX(fwd);
            y1 += HexPointUtil::DeltaY(fwd);
        }
    }
    ComputeRingGodel();
}

//----------------------------------------------------------------------------

void Pattern::ComputeRingGodel()
{
    for (int i = 0; i < NUM_SLICES; i++)
    {
        m_ring_godel[i].SetEmpty();
        for (int s = 0; s < NUM_SLICES; s++)
        {
            int j = (i + s) % NUM_SLICES;
            if ((m_slice[j][FEATURE_CELLS] & 1) == 1)
            {
                m_ring_godel[i].AddSliceToMask(s);
                HexColor color = EMPTY;
                if ((m_slice[j][FEATURE_BLACK] & 1) == 1)
                    color = BLACK;
                else if ((m_slice[j][FEATURE_WHITE] & 1) == 1)
                    color = WHITE;
                m_ring_godel[i].SetSliceToColor(s, color);
            }
        }
    }
}

//----------------------------------------------------------------------------

void Pattern::LoadPatternsFromStream(std::istream& f,
                                     std::vector<Pattern>& out)
{
    std::string name;
    std::string comment;
    bool foundName = false;
    bool requiresMirror = false;
    std::size_t lineNumber = 0;
    while (true)
    {
        lineNumber++;
        std::string line;
        if (!getline(f, line))
            break;
        size_t a = line.find('[');
        size_t b =  line.find('/');
        size_t c =  line.find(']');
        if (a<b && b<c && c != std::string::npos)
        {
            if (!foundName)
            {
                name = line.substr(a+1,b-a-1);
                comment = line.substr(b+1,c-b-1);
                foundName = true;
            }
            else
                requiresMirror = true;
        }
        if (line[0] == Pattern::EITHER_FILLIN ||
            line[0] == Pattern::WHITE_FILLIN ||
            line[0] == Pattern::WHITE_STRONG_REVERSIBLE ||
            line[0] == Pattern::WHITE_THREAT_REVERSIBLE ||
            line[0] == Pattern::WHITE_INFERIOR ||
            line[0] == Pattern::WHITE_REVERSIBLE ||
            line[0] == Pattern::VC_CAPTURE)
        //line[0] == Pattern::MOHEX ||
        //line[0] == Pattern::SHIFT ||
        {
            Pattern p;
            if (p.Unserialize(line))
            {
                p.SetName(name);
                p.SetComment(comment);
                out.push_back(p);
                if (requiresMirror)
                {
                    p.Mirror();
                    p.SetName(name + "m");
                    out.push_back(p);
                }
            }
            else
            {
                throw SgException("Error parsing pattern: line "
                                  + std::to_string(lineNumber));
            }
            foundName = false;
            requiresMirror = false;
        }
    }
}

void Pattern::LoadPatternsFromFile(const char* filename,
                                   std::vector<Pattern>& out)
{
    std::ifstream f(filename);
    if (!f)
        throw SgException(std::string("Could not open file for reading: '")
                          + filename + "'\n");
    else
    {
        try {
            LoadPatternsFromStream(f, out);
        }
        catch (QException& e) {
            throw SgException(std::string("Pattern: \'") +
                              filename + "\': "
                              + e.what() + '\n');
        }
    }
}

//----------------------------------------------------------------------------

int PatternUtil::GetExtensionFromGodel(int godel)
{
    for (int r = 1; r <= Pattern::MAX_EXTENSION; r++)
    {
        int mask = ~((1<<(r*(r+1)/2))-1);
        if ((godel & mask) == 0)
            return r;
    }
    Q_ASSERT(false);
    return Pattern::MAX_EXTENSION;
}

//----------------------------------------------------------------------------

