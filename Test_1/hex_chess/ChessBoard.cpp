#include "ChessBoard.h"
#include "GameDebug.h"
#include "GamePvP.h"
#include "GamePvE.h"
#include "GameEvE.h"

#include <QPushButton>

#include <QPainterPath>
#include <QMouseEvent>
#include <QPainter>
#include <QThread>
#include <QLabel>
#include <QtMath>
#include <QTimer>

#include "ShareData.h"
#include "HexLog.h"
#include "HexTimer.h"

#define _BLACK_ QColor(0, 0, 0, 255)
#define _WHITE_ QColor(255, 255, 255, 255)
#define _RED_ QColor(234, 67, 53, 255)
#define _BLUE_ QColor(66, 133, 244, 255)
#define _GRAY_ QColor(191, 191, 191, 255)
#define _YELLOW_ QColor(255, 225, 135, 255)

#define _RED_T_ QColor(234, 67, 53, 127)
#define _BLUE_T_ QColor(66, 133, 244, 127)

#define _RADIO 1
#define _BORDER_RH 0.25
#define _FONT_NAME "JetBrains Mono NL"


ChessBoard::ChessBoard(int Order, bool First, int GMD, QWidget *parent)
    : QWidget(parent)
    , order(Order)
    , isPlayer(First)
    , AIThread(new QThread())
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
    , board(new HexMatrix(order))
    , points(new QVector<QVector<QPointF>>(pointsRows, QVector<QPointF>(pointsCols)))
    , coordPoints1(new QVector<QPointF>(order<<1))
    , coordPoints2(new QVector<QPointF>(order<<1))
    , winnerRoute(new QVector<HexLocation>)
    , borderPath_ud(new QPainterPath)
    , borderPath_lr(new QPainterPath)
    , gridPath(new QPainterPath)
    , winnerPath(new QPainterPath)
{
    this->setMinimumSize((boardBaseSize * 20).toSize());
    this->setMouseTracking(true);

    switch (static_cast<_GMode>(GMD))
    {
    case _GMode::_PvP:
        gameMode = new GamePvP(&isEnd, board, winnerRoute, &attacker);
        break;
    case _GMode::_PvE:
        gameMode = new GamePvE(&isEnd, board, winnerRoute, &attacker, isPlayer);
        connect(this, &ChessBoard::AIWorking, this, &ChessBoard::StartTime);
        connect(this, &ChessBoard::AIWorking, gameMode, &GameMode::AIWork);
        connect(gameMode, &GameMode::placeChess, this, [=](int _row, int _col) {
            ai_is_working = false;
            hexLog() << hst::plyer;
            (*attacker ? whiteTimer : blackTimer)->Stop();
            PlaceChessPieces(_row, _col);
        });
        AIThread->start();
        break;
    case _GMode::_EvE:
        gameMode = new GameEvE(&isEnd, board, winnerRoute, &attacker);
        connect(this, &ChessBoard::AIWorking, this, &ChessBoard::StartTime);
        connect(this, &ChessBoard::AIWorking, gameMode, &GameMode::AIWork);
        connect(gameMode, &GameMode::placeChess, this, [=](int _row, int _col) {
            ai_is_working = false;
            hexLog() << hst::plyer;
            (*attacker ? whiteTimer : blackTimer)->Stop();
            PlaceChessPieces(_row, _col);
        });
        AIThread->start();
        isPlayer = false;
        break;
    case _GMode::_Debug:
        debug = true;
        gameMode = new GameDebug(&isEnd, board, winnerRoute, &attacker);
        connect(this, &ChessBoard::AIWorking, this, &ChessBoard::StartTime);
        connect(this, &ChessBoard::AIWorking, gameMode, &GameMode::AIWork);
        connect(gameMode, &GameMode::placeChess, this, [=](int _row, int _col) {
            if (_row == 250) {
                initFinished = true;
                hexLog() << "init finished!" << hlg::endl << hlg::ln;
                return;
            }
            ai_is_working = false;
            afterWorking = true;
            (*attacker ? whiteTimer : blackTimer)->Stop();
            hexLog() << hst::plyer;
            if (_row == 254) return;
            PlaceChessPieces(_row, _col);
        });
        connect(this, &ChessBoard::setPieces, (GameDebug*)gameMode, &GameDebug::AddHistory);
        connect(this, &ChessBoard::RegretAMove, (GameDebug*)gameMode, &GameDebug::RegretAMove);
        connect(this, &ChessBoard::isExit, (GameDebug*)gameMode, &GameDebug::Exit);
        connect(this, &ChessBoard::needInit, (GameDebug*)gameMode, &GameDebug::Init);
        AIThread->start();
        isPlayer = true;
        break;
    case _GMode::_END:
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
        Test = new QPushButton("AI Work", this);
        Test->setGeometry(0, 0, 200, 80);
        Test->setFont(QFont(fontName, 24));
        connect(Test, &QPushButton::clicked, this, [=]() {
            emit AIWorking();
            Test->setHidden(true);
        });
    }

    his().resize(0);
    hisMove().resize(0);
    hexLog() << "now is initing AI!" << hlg::endl << hlg::ln;

    blackTimer = new HexTimer();
    whiteTimer = new HexTimer();
    timerLabel1 = new QLabel(this);
    timerLabel2 = new QLabel(this);
    timerLabel1->setStyleSheet("color: rgb(234, 67, 53)");
    timerLabel2->setStyleSheet("color: rgb(66, 133, 244)");

    myTimer = new QTimer(this);
    myTimer->setInterval(125);
    connect(myTimer, &QTimer::timeout, this, [this]() {
        if (ai_is_working) {
            if (*attacker)
                timerLabel2->setText(whiteTimer->Str());
            else timerLabel1->setText(blackTimer->Str());
        }
        if (afterWorking) {
            afterWorking = false;
            if (!*attacker)
                timerLabel2->setText(whiteTimer->Str());
            else timerLabel1->setText(blackTimer->Str());
        }
    });
    myTimer->start();

//    if (debug && order == 11 && _PUCT_ON_)
//    {
//        showValue = true;
//        QPushButton *uv = new QPushButton("update value", this);
//        uv->setFont(QFont(_FONT_NAME));
//        uv->move(0, 100);
//        connect(uv, &QPushButton::clicked, this, [this]() {
//            static int i = 0;
//            qDebug() << i++;
//            if (i == 120) i = 0;
//            UpdateValue();
//            update();
//        });
//        model_init();
//    }
}

ChessBoard::~ChessBoard()
{
//    if (!ai_is_working)
    {
        emit isExit();
        while(!rdy());
    }
    AIThread->exit();
    AIThread->wait();
    delete gameMode;
    delete AIThread;

    delete empty;
    delete black;
    delete white;
    delete black_t;
    delete white_t;

    delete points;
    delete coordPoints1;
    delete coordPoints2;
    delete winnerRoute;
    delete borderPath_ud;
    delete borderPath_lr;
    delete gridPath;
    delete winnerPath;
    
    delete board;
    delete blackTimer;
    delete whiteTimer;
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

    // Update Timer Label
    timerLabel1->setFont(coord_f2);
    timerLabel2->setFont(coord_f2);
    timerLabel1->move(
        ((*points)[pointsRows-1][pointsCols-1] - QPointF(
            gridWidth * order * 0.35,
            gridHeight * (order + 0.5) * 0.75
        )).toPoint()
    );
    timerLabel2->move(
        ((*points)[pointsRows-1][pointsCols-1] - QPointF(
            gridWidth * order * 0.35,
            gridHeight * (order - 0.5) * 0.75
        )).toPoint()
    );
    timerLabel1->resize(gridWidth * order * 0.35, gridHeight * 0.75);
    timerLabel2->resize(gridWidth * order * 0.35, gridHeight * 0.75);
}

void ChessBoard::paintEvent(QPaintEvent *event)
{
    PaintChessBoard();
    PaintPiecesPlaced();
    if ((isPlayer || debug) && !isEnd)
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
    coord_row = mouse_row == -1 ? ' ' : 11 - mouse_row;
    coord_col = mouse_col == -1 ? ' ' : mouse_col + 65;
    if (_mr != mouse_row || _mc != mouse_col)
    {
        update();
    }
}

void ChessBoard::mousePressEvent(QMouseEvent *event)
{
    if ((isPlayer || debug) && !isEnd)
    {
        press_row = mouse_row;
        press_col = mouse_col;
    }
}

void ChessBoard::mouseReleaseEvent(QMouseEvent *event)
{
    if (ai_is_working)
    {
        hexLog() << hst::aiwarn;
        return;
    }
    if (!isEnd && press_row == mouse_row && press_col == mouse_col)
    {
        if (0 <= press_row && press_row < order && 0 <= press_col && press_col <= order)
        {
            if (board->GetCell(press_row, press_col) == HexCell::Empty)
            {
                hexLog() << "Player set" << (*attacker ? "white" : "black")
                         << HexLocation(press_row, press_col).Str()
                         << (*attacker == HexAttacker::Black ? hlg::bdl : hlg::wdl)
                         << hlg::ln;
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
    int begin = order < 10 ? 0 : order - 9;
    int _col = order << 1;
    for (int i = begin; i < order; i++)
    {
        int _i = i << 1;
        (*coordPoints1)[_i] = (*points)[i+1][0] - QPointF(gridWidth / 3, gridHeight / 6);
        (*coordPoints2)[_i] = (*points)[i+1][_col] - QPointF(-gridWidth / 8, gridHeight / 12);
        _i++;
        (*coordPoints1)[_i] = (*points)[0][_i] - QPointF(0, gridHeight / 6);
        (*coordPoints2)[_i] = (*points)[order][_i] + QPointF(gridWidth / 3, gridHeight / 6);
    }
    for (int i = 0; i < begin; i++)
    {
        int _i = i << 1;
        (*coordPoints1)[_i] = (*points)[i+1][0] - QPointF(gridWidth * 0.465, gridHeight / 6);
        (*coordPoints2)[_i] = (*points)[i+1][_col] - QPointF(-gridWidth / 40, gridHeight / 12);
        _i++;
        (*coordPoints1)[_i] = (*points)[0][_i] - QPointF(0, gridHeight / 6);
        (*coordPoints2)[_i] = (*points)[order][_i] + QPointF(gridWidth / 3, gridHeight / 6);
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
            if (board->GetCell(i, j) != HexCell::Empty)
            {
                painter.setBrush(*_c[static_cast<int>(board->GetCell(i, j))]);
                painter.drawEllipse(centre + add * j, radius_solid, radius_solid);
            }
        }
    }

    // painter count
    painter.setPen(_BLACK_);
    painter.setFont(coord_f1);
    for (int i = 0, end = hisMove().size(); i < end; i++)
    {
        int _r = hisMove()[i] / 11 + 1;
        int _c = (hisMove()[i] % 11) << 1;
        if (i < 9) {
            painter.drawText(
                (*points)[_r][_c] + QPointF(gridWidth * 0.38, -gridHeight / 8),
                QString::number(i+1));
        } else if (i < 99) {
            painter.drawText(
                (*points)[_r][_c] + QPointF(gridWidth * 0.263, -gridHeight / 8),
                QString::number(i+1));
        } else {
            painter.drawText(
                (*points)[_r][_c] + QPointF(gridWidth * 0.146, -gridHeight / 8),
                QString::number(i+1));
        }
    }
    painter.end(); // end
}

void ChessBoard::PaintProjection()
{
    int _row = mouse_row;
    int _col = mouse_col;
    if (_row != -1 && board->GetCell(_row, _col) == HexCell::Empty)
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
        painter.drawText((*coordPoints1)[_i], QString::number(order-i));
        painter.drawText((*coordPoints2)[_i], QString::number(order-i));
        painter.drawText((*coordPoints1)[++_i], (*coord_char)[i]);
        painter.drawText((*coordPoints2)[_i], (*coord_char)[i]);
    }
    painter.setFont(coord_f2);
    painter.setPen(white->color());
    painter.drawText(
        coord_label_p2,
        QString("row: ") + (coord_row == ' ' ? "" : QString::number(coord_row))
    );
    painter.setPen(black->color());
    painter.drawText(
        coord_label_p1,
        QString("col: ") + coord_col
    );
    painter.end(); // end
}


void ChessBoard::PlaceChessPieces(int row, int col)
{
    if (debug && row == -1 && col == -1)
    {
        update();
        return;
    }
    Q_ASSERT((0 <= row && row < order && 0 <= col && col <= order));
    Q_ASSERT(board->GetCell(row, col) == HexCell::Empty);
    board->Placed(row, col, attacker);
    emit setPieces(row, col);
    ConditionsDetermine();
    update();
    if (!initFinished && quick && quickAttacker)
    {
        hexLog() << "ai init not finished, agent try move failed"
                 << (*attacker ? hlg::wdl : hlg::bdl);
    }
    if (!isEnd &&
        ((!isPlayer && !debug) ||
        (demo) ||
        (initFinished && quick && quickAttacker))
       )
    {
        ai_is_working = true;
        emit AIWorking();
    }
}

void ChessBoard::ConditionsDetermine()
{
    gameMode->Determine();
    if (isEnd)
    {
        hexLog() << "The winer is :" << (*attacker ? "White" : "Black")
               << (*attacker == HexAttacker::Black ? hlg::bdl : hlg::wdl);
        demo_stop();
        UpdateWinnerPath();
        wner() = *attacker;
        emit GameOver(*attacker);
        return;
    }
    if (quick) quickAttacker = !quickAttacker;
    attacker = !attacker;
    isPlayer = gameMode->IsPlayer();

}

void ChessBoard::StartTime()
{
    if (initFinished)
        (*attacker ? whiteTimer : blackTimer)->Start();
}


void ChessBoard::ai_quick(bool c)
{
    quick = c;
    if (!quick)
    {
        quickAttacker = false;
    }
    else
    {
        hexLog() << "agent represented"
                 << (*attacker ? "Black" : "White")
                 << (*attacker ? hlg::bdl : hlg::wdl);
    }
}

void ChessBoard::ai_move()
{
    if (!initFinished)
    {
        hexLog() << "init not finished!" << hlg::endl << hlg::ln;
        return;
    }
    if (!ai_is_working && !isEnd)
    {
        ai_is_working = true;
        emit AIWorking();
    }
    else if(ai_is_working) hexLog() << hst::aiwarn;
}

void ChessBoard::ai_stop()
{
    static_cast<GameDebug*>(gameMode)->StopAI();
}

void ChessBoard::ai_demo()
{
    if (!ai_is_working && !isEnd && initFinished)
    {
        ai_is_working = true;
        emit AIWorking();
    }
    demo = true;
    static_cast<GameDebug*>(gameMode)->dm = "Demo:";
}

void ChessBoard::demo_stop()
{
    demo = false;
    ai_stop();
    static_cast<GameDebug*>(gameMode)->dm = "";
}

void ChessBoard::regret_a_move()
{
    if (!board->PiecesNum())
    { hexLog() << "board is empty" << hlg::endl << hlg::ln; return; }
    if (!ai_is_working && !isEnd)
    {
        ai_is_working = true;
        attacker = !attacker;
        if (quick) quickAttacker = !quickAttacker;
        emit RegretAMove();
    }
    else if(ai_is_working) hexLog() << hst::aiwarn;
}

void ChessBoard::need_init()
{
    emit needInit();
}


