#include "ChessBoard.h"
#include "GamePvP.h"
#include "GamePvE.h"
#include "GameEvE.h"

#include <QPushButton>

#include <QPainterPath>
#include <QMouseEvent>
#include <QPainter>
#include <QThread>
#include <QtMath>

#define _GMODE _GMode::_EvE
#define _ORDER 11
#define _RADIO 1
#define _FIRST 1
#define _BORDER_RH 0.25
#define _FONT_NAME "JetBrains Mono NL"

ChessBoard::ChessBoard(QWidget *parent)
    : QWidget(parent)
    , AIThread(new QThread())
    , isPlayer(_FIRST)
    , order(_ORDER)
    , bottom(order)
    , right((order << 1) + 1)
    , pointsRows(bottom + 1)
    , pointsCols(right + 1)
    , boardBaseSize(qSqrt(3)*((order+(order-1)/2.0+4.0/3)), ((order>>1)*3+(order&1?3:1.5)))
    , coord_char(new QVector<QChar>)
    , radio(_RADIO)
    , border_rh(_BORDER_RH)
    , winner(new QBrush(_YELLOW_, Qt::SolidPattern))
    , empty(new QBrush(_GRAY_, Qt::SolidPattern))
    , black(new QBrush(_RED_, Qt::SolidPattern))
    , white(new QBrush(_BLUE_, Qt::SolidPattern))
    , black_t(new QBrush(_RED_T_, Qt::SolidPattern))
    , white_t(new QBrush(_BLUE_T_, Qt::SolidPattern))
    , fontName(_FONT_NAME)
    , match(new HexMatch(order))
    , points(new QVector<QVector<QPointF>>(pointsRows, QVector<QPointF>(pointsCols)))
    , coordPoints(new QVector<QPointF>(order<<1))
    , winnerRoute(new QVector<HexPoint>)
    , borderPath_ud(new QPainterPath)
    , borderPath_lr(new QPainterPath)
    , gridPath(new QPainterPath)
    , winnerPath(new QPainterPath)
{
    this->setGeometry(550, 200, 600, 600);
    this->setMouseTracking(true);
//    qDebug() << QThread::idealThreadCount();
    switch (_GMode(_GMODE))
    {
    case _GMode::_PvP:
        gameMode = new GamePvP(&isEnd, match, winnerRoute, &attacker);
        break;
    case _GMode::_PvE:
        gameMode = new GamePvE(&isEnd, match, winnerRoute, &attacker, isPlayer);
        connect(this, &ChessBoard::AIWorking, gameMode, &GameMode::AIWork);
        connect(gameMode, &GameMode::placeChess, this, [=](int _row, int _col) {
            PlaceChessPieces(_row, _col);
        });
        AIThread->start();
        break;
    case _GMode::_EvE:
        gameMode = new GameEvE(&isEnd, match, winnerRoute, &attacker);
        connect(this, &ChessBoard::AIWorking, gameMode, &GameMode::AIWork);
        connect(gameMode, &GameMode::placeChess, this, [=](int _row, int _col) {
            PlaceChessPieces(_row, _col);
        });
        AIThread->start();
        isPlayer = false;
        break;
    }
    gameMode->moveToThread(AIThread);

    for (int i = 0; i < order; i++)
    {
        coord_char->push_back(QChar(i+65));
    }

    borderPath_ud->setFillRule(Qt::WindingFill);
    borderPath_lr->setFillRule(Qt::WindingFill);
    gridPath->setFillRule(Qt::OddEvenFill);

    coord_f1.setFamily(fontName);
    coord_f2.setFamily(fontName);

    if (!isPlayer)
    {
        Test = new QPushButton("AI First", this);
        Test->setGeometry(0, 0, 200, 80);
        Test->setFont(QFont(fontName, 24));
        connect(Test, &QPushButton::clicked, this, [=]() {
            emit AIWorking();
            Test->setHidden(true);
        });
    }
}

ChessBoard::~ChessBoard()
{
    delete gameMode;
    AIThread->exit();
    AIThread->wait();
    delete AIThread;

    delete empty;
    delete black;
    delete white;
    delete black_t;
    delete white_t;

    delete points;
    delete coordPoints;
    delete winnerRoute;
    delete borderPath_ud;
    delete borderPath_lr;
    delete gridPath;
    delete winnerPath;
    
    delete match;
}

void ChessBoard::resizeEvent(QResizeEvent *event)
{
    // calculate scale ratio
    double scaleRatio = radio * qMin(
        this->width() / boardBaseSize.width(),
        this->height() / boardBaseSize.height()
    );

    gridWidth = scaleRatio * qSqrt(3);
    gridHeight = scaleRatio * 2;

    radius_solid = gridWidth * 0.4;
    radius_dotted = gridWidth * 0.44;

    ulc_x = (this->width() - scaleRatio * boardBaseSize.width()) / 2 + gridWidth * 2 / 3;
    ulc_y = (this->height() - scaleRatio * boardBaseSize.height()) / 2 + gridHeight / 4;

    UpdatePoints();
    UpdateBorderPath();
    UpdateGridPath();
    UpdateWinnerPath();
    UpdateCoordPoints();
}

void ChessBoard::paintEvent(QPaintEvent *event)
{
    PaintChessBoard();
    PaintPiecesPlaced();
    if (isPlayer && !isEnd)
    {
        PaintProjection();
    }
    PaintOtherComponents();
}

void ChessBoard::mouseMoveEvent(QMouseEvent *event)
{
    int _mr = mouse_row;
    int _mc = mouse_col;
    int _rx = event->pos().x() - ulc_x;
    int _ry = event->pos().y() - ulc_y;
    int _row = (_ry << 2) / gridHeight;
    int point_row = _row / 3;
    int point_col = (_rx << 1) / gridWidth - point_row + 1;

    if (0 > point_row || point_row > order || 0 >= _ry
        || 0 >= point_col || point_col > order << 1 || 0 >= _rx)
    {
        mouse_row = mouse_col = -1;
        coord_row = coord_col = ' ';
        update();
        return;
    }

    auto _nonInTriangle = [&](int _row, int _col)
    {
        QPoint _P(event->pos());
        QPointF _AP = _P - (*points)[_row][_col-1];
        QPointF _BP = _P - (*points)[_row][_col];
        QPointF _CP = _P - (*points)[_row][_col+1];
        auto _cross = [](QPointF _v1, QPointF _v2)
        {
            return _v1.x() * _v2.y() - _v1.y() * _v2.x();
        };
        int _c1 = _cross(_AP, _BP);
        int _c2 = _cross(_BP, _CP);
        int _c3 = _cross(_CP, _AP);
        return _c1*_c2 < 0 && _c2*_c3 < 0;
    };

    // 01 10: upper, 00 11: lower, where in triangle and point_col is odd
    bool nit;
    bool odd = point_col & 1;
    bool correction; // correct a bug of mous_col
    mouse_row = point_row - ((_row % 3 == 0) ?
        correction = (!odd) &
        (nit = _nonInTriangle(point_row, point_col)),
        (nit ^ odd) : (correction = 0)
    );
    mouse_col = ((point_col - 1) >> 1) + correction;
    mouse_row = (mouse_row == -1
        || mouse_row == order
        || mouse_col == order
    ) ? (mouse_col = -1) : mouse_row;
    coord_row = mouse_row == -1 ? ' ' : mouse_row + 65;
    coord_col = mouse_col == -1 ? ' ' : mouse_col;
    if (_mr != mouse_row || _mc != mouse_col)
    {
        update();
    }
}

void ChessBoard::mousePressEvent(QMouseEvent *event)
{
    if (isPlayer && !isEnd)
    {
        press_row = mouse_row;
        press_col = mouse_col;
    }
}

void ChessBoard::mouseReleaseEvent(QMouseEvent *event)
{
    if (!isEnd && press_row == mouse_row && press_col == mouse_col)
    {
        if (0 <= press_row && press_row < order && 0 <= press_col && press_col <= order)
        {
            if (match->GetCell(press_row, press_col) == HexCell::Empty)
            {
                PlaceChessPieces(press_row, press_col);
            }
        }
    }
}


void ChessBoard::UpdatePoints()
{
    double _ye = ulc_y;
    double _yo = ulc_y + gridHeight / 4;
    for (int i = 0; i < pointsRows; i++)
    {
        double _xe = ulc_x + gridWidth * (i - 1) / 2;
        double _xo = ulc_x + gridWidth * i / 2;
        for (int j = 0; j < pointsCols; j++)
        {
            (*points)[i][j] = QPointF(_xe, _ye);
            (*points)[i][++j] = QPointF(_xo, _yo);
            _xe += gridWidth;
            _xo += gridWidth;
        }
        _ye += gridHeight * 0.75;
        _yo += gridHeight * 0.75;
    }
}

void ChessBoard::UpdateBorderPath()
{
    borderPath_ud->clear();
    borderPath_lr->clear();

    auto _pathTo = [&](QPainterPath *path,QPointF _a_and_d, QPointF _b,  QPointF _c)
    {
        QPointF _a = (*points)[0][0] + _a_and_d;
        QPointF _d = (*points)[bottom][right] - _a_and_d;
        path->moveTo((*points)[0][0]);
        path->lineTo(_a);
        path->lineTo(_b);
        path->lineTo(_c);
        path->lineTo(_d);
        path->lineTo((*points)[bottom][right]);
        path->closeSubpath();
    };

    // move offset, grid's width : height = 2w : 3h
    // offset of right top point need move (-1/6 w, -1/4 h) to centre of upper right corner
    const QPointF _border(gridWidth * border_rh * 2 / 3, - gridHeight * border_rh);
    const QPointF _border_P(_border - QPointF(gridWidth / 6, gridHeight / 4));
    const QPointF _b = (*points)[0][right] + _border_P;
    const QPointF _c = (*points)[bottom][0] - _border_P;

    _pathTo(borderPath_ud, _border, _b, _c);
    _pathTo(borderPath_lr, -_border, _c, _b);
}

void ChessBoard::UpdateGridPath()
{
    gridPath->clear();
    auto _line = [&](int _ulc_r, int _ulc_c)
    {
        gridPath->moveTo((*points)[_ulc_r][_ulc_c]);
        gridPath->lineTo((*points)[_ulc_r][++_ulc_c]);
        gridPath->lineTo((*points)[_ulc_r][_ulc_c + 1]);
        gridPath->lineTo((*points)[++_ulc_r][_ulc_c]);
        gridPath->lineTo((*points)[_ulc_r][--_ulc_c]);
        gridPath->lineTo((*points)[_ulc_r][--_ulc_c]);
        gridPath->closeSubpath();
    };
    for (int i = 0; i < order; i++)
    {
        for (int j = 0; j < order; j++)
        {
            _line(i, (j << 1) + 1);
        }
    }
}

void ChessBoard::UpdateWinnerPath()
{
    if (isEnd)
    {
        winnerPath->clear();
        auto _line = [&](int _ulc_r, int _ulc_c)
        {
            winnerPath->moveTo((*points)[_ulc_r][_ulc_c]);
            winnerPath->lineTo((*points)[_ulc_r][++_ulc_c]);
            winnerPath->lineTo((*points)[_ulc_r][_ulc_c + 1]);
            winnerPath->lineTo((*points)[++_ulc_r][_ulc_c]);
            winnerPath->lineTo((*points)[_ulc_r][--_ulc_c]);
            winnerPath->lineTo((*points)[_ulc_r][--_ulc_c]);
            winnerPath->closeSubpath();
        };
        for (const auto& [r, c] : *winnerRoute)
        {
            _line(r, (c << 1) + 1);
        }
    }
}

void ChessBoard::UpdateCoordPoints()
{
    coord_f1.setPointSizeF(gridHeight / 4);
    coord_f2.setPointSizeF(gridWidth * order / 25);
    coord_label_p1 = (*points)[0][0] + QPointF(0, gridHeight * (order + 0.5) * 0.75);
    coord_label_p2 = coord_label_p1 + QPointF(gridWidth * order / 4, 0);
    int end = order < 10 ? order : 10;
    for (int i = 0; i < end; i++)
    {
        int _i = i << 1;
        (*coordPoints)[_i++] = (*points)[i+1][0] - QPointF(gridWidth / 3, gridHeight / 6);
        (*coordPoints)[_i] = (*points)[0][_i] - QPointF(0, gridHeight / 6);
    }
    for (int i = end; i < order; i++)
    {
        int _i = i << 1;
        (*coordPoints)[_i++] = (*points)[i+1][0] - QPointF(gridWidth / 3, gridHeight / 6);
        (*coordPoints)[_i] = (*points)[0][_i] - QPointF(gridWidth / 9, gridHeight / 6);
    }
}

void ChessBoard::PaintChessBoard()
{
    QPainter painter(this);

    // paint back side
    painter.setPen(QPen(Qt::transparent, 0));
    painter.setBrush(*black);
    painter.drawPath(*borderPath_ud);
    painter.setBrush(*white);
    painter.drawPath(*borderPath_lr);

    // paint board's grids
    painter.setPen(QPen(Qt::black, 2));
    painter.setBrush(*empty);
    painter.drawPath(*gridPath);

    // paint winner chess path
    if (isEnd)
    {
//        painter.setPen(QPen(Qt::transparent, 0));
        painter.setBrush(*winner);
        painter.drawPath(*winnerPath);
    }

    painter.end(); // end
}

void ChessBoard::PaintPiecesPlaced()
{
    QPainter painter(this);
    painter.setPen(QPen(Qt::transparent, 0));
    QPointF add(gridWidth, 0);
    QBrush *_c[2]{black, white};
    for (int i = 0; i < order; i++)
    {
        QPointF centre = ((*points)[i][1] + (*points)[i+1][2]) / 2;
        for (int j = 0; j < order; j++)
        {
            if (match->GetCell(i, j) != HexCell::Empty)
            {
                painter.setBrush(*_c[static_cast<int>(match->GetCell(i, j))]);
//                painter.setBrush((*pieces)[i][j] != _Piece::First ? *second : *first);
                painter.drawEllipse(centre + add * j, radius_solid, radius_solid);
            }
//            centre += add;
        }
    }
    painter.end(); // end
}

void ChessBoard::PaintProjection()
{
    int _row = mouse_row;
    int _col = mouse_col;
    if (_row != -1 && match->GetCell(_row, _col) == HexCell::Empty)
    {
        _col <<= 1;
        QPainter painter(this);
        painter.setPen(QPen(Qt::transparent, 0));
        painter.setBrush(*attacker ? *white_t : *black_t);
        QPointF centre = ((*points)[_row][_col+1] + (*points)[_row+1][_col+2]) / 2;
        painter.drawEllipse(centre, radius_dotted, radius_dotted);
        painter.end(); // end
    }
}

void ChessBoard::PaintOtherComponents()
{
    QPainter painter(this);
    painter.setFont(coord_f1);
    painter.setPen(_BLACK_);
    for (int i = 0; i < order; i++)
    {
        int _i = i << 1;
        painter.drawText((*coordPoints)[_i], (*coord_char)[i]);
        painter.drawText((*coordPoints)[++_i], QString::number(i));
    }
    painter.setFont(coord_f2);
    painter.setPen(white->color());
    painter.drawText(coord_label_p1, QString("row: ") + coord_row);
    painter.setPen(black->color());
    painter.drawText(coord_label_p2,
        QString("col: ") +(coord_col == ' ' ? "" : QString::number(coord_col))
    );
    painter.end(); // end
}


void ChessBoard::PlaceChessPieces(int row, int col)
{
    Q_ASSERT((0 <= row && row < order && 0 <= col && col <= order));
    Q_ASSERT(match->GetCell(row, col) == HexCell::Empty);
    match->GetCell(row, col) = static_cast<HexCell>(attacker);
    emit setPieces(row, col);
    qDebug() << (*attacker ? "White:" : "Black:")
        << "(" << row << "," << col << ")" << Qt::endl;
    ConditionsDetermine();
    update();
    if (!isPlayer && !isEnd)
    {
        emit AIWorking();
    }
}

void ChessBoard::ConditionsDetermine()
{
    gameMode->Determine();
    if (isEnd)
    {
        qDebug() << "The winer is :" << (*attacker ? "White" : "Black");
        UpdateWinnerPath();
        return;
    }
    attacker = !attacker;
    isPlayer = gameMode->IsPlayer();

}


