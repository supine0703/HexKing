#ifndef INFERIORCELL_HPP
#define INFERIORCELL_HPP

#include <iostream>
#include <cstring>
#include <chrono>
#include <climits>
#include <tuple>

using namespace std;
using namespace std::chrono;

//#include "qdebug.h"
#include <sstream>

#include <QDebug>

#include "MctsWork.h"
#include "HexMatch.hpp"

using namespace std;
using namespace std::chrono;

#define USE_COLOR true
// Unsure of what the states are represented as
//#define PLAYER_1_COLOUR 1
//#define PLAYER_2_COLOUR -1
//#define EMPTY_COLOUR 0

// [pattern number][Red Or Blue][Coordinate pair][Coordinate]
#define PATTERN_TOT 3 * 6
#define PATTERN_MAX_COORDS 4
#define X -100

#define EDGE_TOT 6
#define EDGE_MAX_COORDS 2

class InferiorCell
{
public:
    InferiorCell(const HexMatch& _board, uint8_t _order)
    {
        board = &_board;
        uint8_t order = _order;
    }
    enum Tile {
        PLAYER_BLACK = 0, // Black
        PLAYER_WHITE = 1, // White
        EMPTY_COLOUR = 2
    };
    const int patterns[PATTERN_TOT][2][PATTERN_MAX_COORDS][2] = {
        // First Pattern rotated
        {
            {{-1, 0}, {0,-1}, {1,-1}, {1, 0}},
            {{ X, X}, {X, X}, {X, X}, {X, X}}
        },
        {
            {{0, -1}, {1,-1}, {1, 0}, {0, 1}},
            {{ X, X}, {X, X}, {X, X}, {X, X}}
        },
        {
            {{1, -1}, {1, 0}, {0, 1}, {-1, 1}},
            {{ X, X}, {X, X}, {X, X}, {X, X}}
        },
        {
            {{1, 0}, {0, 1}, {-1, 1}, {-1, 0}},
            {{ X, X}, {X, X}, {X, X}, {X, X}}
        },
        {
            {{0, 1}, {-1, 1}, {-1, 0}, {0, -1}},
            {{ X, X}, {X, X}, {X, X}, {X, X}}
        },
        {
            {{-1, 1}, {-1, 0}, {0, -1}, {1, -1}},
            {{ X, X}, {X, X}, {X, X}, {X, X}}
        },
        // Second Pattern rotated
        {
            {{0, -1}, {1, -1}, {1, 0}, {X, X}},
            {{-1, 1}, {X,  X}, {X, X}, {X, X}}
        },
        {
            {{1, -1}, {1, 0}, {0, 1}, {X, X}},
            {{-1, 0}, {X,  X}, {X, X}, {X, X}}
        },
        {
            {{1, 0},  {0, 1}, {-1, 1}, {X, X}},
            {{0, -1}, {X,  X}, {X, X}, {X, X}}
        },
        {
            {{0, 1}, {-1, 1}, {-1, 0}, {X, X}},
            {{1, -1}, {X,  X}, {X, X}, {X, X}}
        },
        {
            {{-1, 1}, {-1, 0}, {0, -1}, {X, X}},
            {{1,  0}, {X,  X}, {X, X}, {X, X}}
        },
        {
            {{-1, 0}, {0, -1}, {1, -1}, {X, X}},
            {{0,  1}, {X,  X}, {X, X}, {X, X}}
        },
        // Third Pattern Rotated
        {
            {{1,-1}, {1, 0}, {X, X}, {X, X}},
            {{-1,0}, {-1,1}, {X, X}, {X, X}}
        },
        {
            {{1, 0}, {0, 1}, {X, X}, {X, X}},
            {{-1,0}, {0, -1}, {X, X}, {X, X}}
        },
        {
            {{0, 1}, {-1, 1}, {X, X}, {X, X}},
            {{0, -1}, {1, -1}, {X, X}, {X, X}}
        },
        {
            {{-1, 1}, {-1, 0}, {X, X}, {X, X}},
            {{1, -1}, {1, 0}, {X, X}, {X, X}}
        },
        {
            {{-1, 0}, {0, -1}, {X, X}, {X, X}},
            {{1, 0}, {0, 1}, {X, X}, {X, X}}
        },
        {
            {{0,-1}, {1, -1}, {X, X}, {X, X}},
            {{0, 1}, {-1, 1}, {X, X}, {X, X}}
        }

    };

    const int edge_pattern[4][EDGE_TOT][2][EDGE_MAX_COORDS][2] = {
        {  // Bottom Edge
            { // pattern 1
                {{-1, 0}, {0, -1}},
                {{X,  X},  {X, X}}
            },
            { // pattern 1 flipped
                {{1, -1}, {1, 0}},
                {{X,  X},  {X, X}}
            },
            { // pattern 2
                {{-1, 0}, {1, 0}},
                {{X,  X}, {X, X}}
            },
            { // pattern 3
                {{1,  0}, {X, X}},
                {{0, -1}, {X, X}}
            },
            { // pattern 3 flipped
                {{-1, 0}, {X, X}},
                {{1,-1}, {X, X}}
            },
            { // pattern 4
                {{X, X}, {X, X}},
                {{0, -1}, {1, -1}}
            }
        },
        {  // Top Edge
            { // pattern 1
                {{-1, 1}, {-1, 0}},
                {{X, X}, {X, X}}
            },
            { // pattern 1 flipped
                {{0, 1}, {1, 0}},
                {{X, X}, {X, X}}
            },
            { // pattern 2
                {{-1, 0}, {1, 0}},
                {{X, X}, {X, X}}
            },
            { // pattern 3
                {{-1, 0}, {X, X}},
                {{0, 1}, {X, X}}
            },
            { // pattern 3 flipped
                {{1, 0}, {X, X}},
                {{-1, 1}, {X, X}}
            },
            { // pattern 4
                {{X, X}, {X, X}},
                {{-1, 1}, {0, 1}}
            }
        },
        {  // Left Edge
            { // pattern 1
                {{X, X}, {X, X}},
                {{0, -1}, {1, -1}}
            },
            { // pattern 1 flipped
                {{X, X}, {X, X}},
                {{1, 0}, {0, 1}}
            },
            { // pattern 2
                {{X, X}, {X, X}},
                {{0, -1}, {0, 1}}
            },
            { // pattern 3
                {{1, -1}, {X, X}},
                {{0,  1}, {X, X}}
            },
            { // pattern 3 flipped
                {{1, 0}, {X, X}},
                {{0, -1}, {X, X}}
            },
            { // pattern 4
                {{1,-1}, {1, 0}},
                {{X, X}, {X, X}}
            }
        },
        {  // Right Edge
            { // pattern 1
                {{X, X}, {X, X}},
                {{0, -1}, {-1, 0}}
            },
            { // pattern 1 flipped
                {{X, X}, {X, X}},
                {{-1, 1}, {0, 1}}
            },
            { // pattern 2
                {{X, X}, {X, X}},
                {{0,-1}, {0, 1}}
            },
            { // pattern 3
                {{-1, 1}, {X, X}},
                {{0,-1}, {X, X}}
            },
            { // pattern 3 flipped
                {{-1, 0}, {X, X}},
                {{0, 1}, {X, X}}
            },
            { // pattern 4
                {{X, X}, {X, X}},
                {{-1, 1}, {-1, 0}}
            }
        }
    };

    bool CheckCorner(int x, int y, const HexMatch board) ;

    bool CheckForDeadCell(int x, int y, const HexMatch board);
private:
    uint8_t order;
    const HexMatch* board;
};

bool InferiorCell:: CheckCorner(int x, int y, const HexMatch board) { // Performs pattern recognition around the edges
    int edgenum;
    if ((x == 0 && y == 0) || (x == 0 && y == order - 1) || (x == order - 1 && y == 0) || (x == order - 1 && y == order - 1)) {
        return false;
    }
    else if (x == 0) {
        edgenum = 2;
    }
    else if (x == order - 1) {
        edgenum = 3;
    }
    else if (y == 0) {
        edgenum = 1;
    }
    else if (y == order - 1) {
        edgenum = 0;
    }
    for (int pIndex = 0; pIndex < EDGE_TOT; pIndex++) {
        bool patternMatched = true;
        for (int c = 0; c < 2; c++) {
            for (int coord = 0; coord < EDGE_MAX_COORDS; coord++) {
                if (edge_pattern[edgenum][pIndex][c][coord][0] != X) {
                    int xCheck = x + edge_pattern[edgenum][pIndex][c][coord][0];
                    int yCheck = y + edge_pattern[edgenum][pIndex][c][coord][1];
                    if (c == 0) {
                        if (board(xCheck, yCheck) != PLAYER_BLACK) patternMatched = false;
                    }
                    else {
                        if (board(xCheck, yCheck) != PLAYER_WHITE) patternMatched = false;
                    }
                }
            }

        }
        if (patternMatched) {
            return true;
        }
    }


    return false;
}


bool InferiorCell:: CheckForDeadCell(int x, int y, const HexMatch board) { // Obviously dont run this on dead cells
    if (x != 0 && x != order - 1 && y != 0 && y != order - 1) {
        int colourSel1 = 0;
        int colourSel2 = 1;
        for (int colourNum = 0; colourNum < 2; colourNum++) {
            for (int patternIndex = 0; patternIndex < PATTERN_TOT; patternIndex++) {

                bool patternMatched = true;
                for (int redCoord = 0; redCoord < PATTERN_MAX_COORDS; redCoord++) {
                    if (patterns[patternIndex][colourSel1][redCoord][0] != X) {
                        int xToCheck = x + patterns[patternIndex][colourSel1][redCoord][0];
                        int yToCheck = y + patterns[patternIndex][colourSel1][redCoord][1];
                        if (!(xToCheck < 0 || yToCheck < 0 || xToCheck >= order || yToCheck >= order)) {
                            if (board(xToCheck, yToCheck) != PLAYER_BLACK) {
                                patternMatched = false;
                                break; // For Performance stop checking
                            }
                        }
                        else {
                            // When Culling against the edge of the board, I assume the pattern is not detected.
                            // Therefore it is possible that this function will return false for a cell which is not Dead
                            // I decided this was preferable to reporting true when the pixel was not dead
                            patternMatched = false;
                            break;
                        }
                    }
                }
                if (patternMatched) { // If devation from the pattern has been seen dont bother continuing checks
                    for (int blueCoord = 0; blueCoord < PATTERN_MAX_COORDS; blueCoord++) {
                        if (patterns[patternIndex][colourSel2][blueCoord][0] != X) {
                            int xToCheck = x + patterns[patternIndex][colourSel2][blueCoord][0];
                            int yToCheck = y + patterns[patternIndex][colourSel2][blueCoord][1];
                            if (!(xToCheck < 0 || yToCheck < 0 || xToCheck >= order || yToCheck >= order)) {
                                if (board(xToCheck, yToCheck) != PLAYER_WHITE) {
                                    patternMatched = false;
                                    break;
                                }
                            }
                            else {
                                patternMatched = false;
                                break;
                            }
                        }
                    }
                }
                if (patternMatched) {
                    return true;
                }

            }
            colourSel1 = 1;
            colourSel2 = 0;
        }
        return false;
    }
    else {
        return CheckCorner(x, y, board);
    }
}

QVector<HexPoint> VisualMove(const HexMatch& board, QVector<HexPoint> occupiedCells)
{
    QVector<HexPoint> Move;
    int end = board.GetOrder();
    for(auto it : occupiedCells)
    {
        int row = it.row;
        int col = it.col;
        if(row - 1 >=0 && col - 1 >= 0 && board(row - 1, col - 1) == HexCell::Empty)
            Move.push_back({row -1, col - 1});
        if(row + 1 < end && col + 1 < end && board(row + 1, col + 1) == HexCell::Empty)
            Move.push_back({row +1, col + 1});
        if(row - 1 >=0 && col + 2 < end && board(row - 1, col + 2) == HexCell::Empty)
            Move.push_back({row -1, col + 2});
        if(row + 1 < end && col - 2 >= 0 && board(row + 1, col - 2) == HexCell::Empty)
            Move.push_back({row + 1, col - 2});
        if(row + 2 < end && col - 1 >= 0  && board(row + 2, col - 1) == HexCell::Empty)
            Move.push_back({row + 2, col - 1});
        if(row - 2 >= 0 && col + 1 < end && board(row - 2, col + 1) == HexCell::Empty)
            Move.push_back({row - 2, col + 1});
    }
    return Move;
}

QVector<HexPoint> Movelimit(const HexMatch& board)
{
    QVector<HexPoint> text;
    QVector<HexPoint> move;
    QVector<HexPoint> occupiedCells;
    InferiorCell* cell = new InferiorCell(board, board.GetOrder());

    int end = board.GetOrder();

    for(int i = 0; i < end; i++)
    {
        for(int j = 0; j < end; j++)
        {
            if (board(i, j) != HexCell::Empty)
            {
                occupiedCells.push_back({i, j});
            }
        }
    }
    qDebug() << occupiedCells.count();

    for(auto it = occupiedCells.begin(); it !=occupiedCells.end(); it++)
    {
        int row = it->row;
        int col = it->col;
        for(int i = col-2; i >= 0 && i <=col + 2 && i < end; i++)
        {
            if(i < col)
            {
                for(int j = row; j <=row + 2 && j < end; j++)
                {
                    if(board(i, j) == HexCell::Empty && !text.contains({i, j}))
                        text.push_back({i, j});
                }
            }
            else if(i > col)
            {
                for(int j = row; j >= 0 && j >=row - 2 ; j--)
                {
                    if(board(i, j) == HexCell::Empty && !text.contains({i, j}))
                        text.push_back({i, j});
                }
            }
            else if(i == col)
            {
                for(int j = row-2; j >= 0 && j <=row + 2 && j < end; j++)
                {
                    if(board(i, j) == HexCell::Empty && !text.contains({i, j}))
                        text.push_back({i, j});
                }
            }
            if(col-1 >= 0 && row-1 >=0)
            {
                if(board(col-1, row-1) == HexCell::Empty && !text.contains({col-1, row-1}))
                    text.push_back({col-1, row-1});
            }
            else if(col+1 < end && row+1 < end)
            {
                if(board(col+1, row+1) == HexCell::Empty && !text.contains({col+1, row+1}))
                    text.push_back({col+1, row+1});
            }
        }
    }
    qDebug() << text.count();

//    for(auto& it : text)
//    {
//        if (!cell->CheckForDeadCell(it.row, it.col, board))
//        {
//            move.push_back({it.row, it.col});
//        }
//    }

    if(move.count() == 0)
    {
        move = text;
    }
    delete cell;
    return move;
}

#endif
