#include "MainWindow.h"

#include "ChessBoard.h"
#include "StartWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow {parent}
    , runWidget(new StartWidget(this))

    , menuBar(new QMenuBar)
    , file(new QMenu("file"))
    , options(new QMenu("options"))
    , order(new QMenu("order"))
    , mode(new QMenu("mode"))
    , function(new QMenu("function"))
{
    this->setWindowIconText("HexKing");
    this->setGeometry(550, 200, 800, 600);
    this->setCentralWidget(runWidget);
    this->setMinimumSize(800, 600);


}

void MainWindow::StartGame(int order, int first, int gmd)
{
    runWidget->deleteLater();
    runWidget = new ChessBoard(order, first, gmd);
    this->setCentralWidget(runWidget);
}



