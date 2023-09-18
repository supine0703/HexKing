#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QWidget>
#include "HexMatch.hpp"

class QPushButton;
class GameMode;
class QThread;

using gmode_t = char;
enum class _GMode : gmode_t
{
    _END = 0,
    _PvP = 1,
    _PvE = 2,
    _EvE = 3
};

class ChessBoard : public QWidget
{
    Q_OBJECT
public:
    ChessBoard(QWidget *parent = nullptr);
    ~ChessBoard();

private:
    QPushButton *Test;

    QThread *AIThread;
    HexAttacker attacker = HexAttacker::Black; // black always play first
    bool isPlayer; // 0: Environment, 1: Player
    bool isEnd = 0;

    // determine when start the game
    const int order;
    const int right;
    const int bottom;
    const int pointsRows;
    const int pointsCols;
    const QSizeF boardBaseSize;
    QVector<QChar> *coord_char;

    // determine where option
    float radio;
    float border_rh; // rh: raido height
    QBrush *winner;
    QBrush *empty;
    QBrush *black;
    QBrush *white;
    QBrush *black_t;
    QBrush *white_t;
    QString fontName;

    // changed as resizeEvent
    double gridWidth;
    double gridHeight;
    double radius_solid;
    double radius_dotted;
    double ulc_x; // Xoffset of upper left corner
    double ulc_y; // Yoffset of upper left corner
    QFont coord_f1;
    QFont coord_f2;
    QPointF coord_label_p1;
    QPointF coord_label_p2;

    // changer about mouseMove
    char mouse_row = -1; // row num of mouse where in grids
    char mouse_col = -1; // col num of mouse where in grids
    char press_row = -1; // row when mouse press
    char press_col = -1; // col when mouse press
    char coord_row = ' '; // row coord of mouse
    char coord_col = ' '; // col coord of mouse


    QVector<QVector<QPointF>> *points;
    QVector<QPointF> *coordPoints;
    QVector<HexPoint> *winnerRoute;
    QPainterPath *borderPath_ud; // up and dowm
    QPainterPath *borderPath_lr; // left and right
    QPainterPath *gridPath;
    QPainterPath *winnerPath;

    HexMatch *match;
    GameMode *gameMode;

    void UpdatePoints();
    void UpdateBorderPath();
    void UpdateGridPath();
    void UpdateWinnerPath();
    void UpdateCoordPoints();
    void PaintChessBoard();
    void PaintPiecesPlaced();
    void PaintProjection();
    void PaintOtherComponents();

    void PlaceChessPieces(int row, int col);
    void ConditionsDetermine();


// QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void setPieces(int row, int col);
    void AIWorking();
};

#endif // CHESSBOARD_H
