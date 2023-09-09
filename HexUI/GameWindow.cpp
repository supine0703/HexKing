#include "configurations"
#include "GameWindow.h"
#include "MenuWidget.h"
#include <QGuiApplication>
#include <QPainter>
#include <QScreen>

GameWindow::GameWindow(QWidget *parent)
    : QMainWindow{parent}
    , runWidget(new MenuWidget(this))
{
    // calculate coordinate of win for set geometry
    QSize resolution(QGuiApplication::primaryScreen()->size());
    int winX = (resolution.width() - WIDTH_START) >> 1;
    int winY = (resolution.height() - HEIGHT_START) * 0.45;
    QRect startWin(winX, winY, WIDTH_START, HEIGHT_START);
    this->setGeometry(startWin);

    this->setWindowIconText("HexKing");
    this->setWindowIcon(QIcon(QPixmap(ICON_WINDOW)));
    this->setMinimumSize(SIZE_MIN_WIN);

    this->connect((MenuWidget*)runWidget, &MenuWidget::quit, this, &GameWindow::close);
}

GameWindow::~GameWindow()
{
    delete runWidget;
}

void GameWindow::resizeEvent(QResizeEvent* event)
{
    // get size of window and background
    int winWidth = this->width();
    int winHeight = this->height();
    int bgWidth = 1;
    int bgHeight = 1;

    // calculate scale ratio
    double scaleRatio = qMin(
        (double)winWidth / bgWidth,
        (double)winHeight / bgHeight
    );

    // calculate size of background
    int scaledWidth = bgWidth * scaleRatio;
    int scaledHeight = bgHeight * scaleRatio;

    // calculate offset of background
    int xOffset = (winWidth - scaledWidth) >> 1;
    int yOffset = (winHeight - scaledHeight) >> 1;

    // paint background
    runWidget->setGeometry(
        QRect(xOffset, yOffset, scaledWidth, scaledHeight)
    );
}





