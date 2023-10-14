#include "MainWindow.h"

#include "ChessBoard.h"
#include "StartWidget.h"
#include "ToolsWidget.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow {parent}
//    , Widget(new QWidget(this))
    , qsplitter(new QSplitter(this))
    , runWidget(new StartWidget(this))
    , toolsWidget(new QWidget(this))
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


    connect(toolsWidget, &ToolsWidget::AIMove, this, [=](){
        emit this->aimove();
    });
}

void MainWindow::StartGame(int order, int first, int gmd)
{

    runWidget->deleteLater();
    toolsWidget->deleteLater();

    runWidget = new ChessBoard(order, first, gmd/*, Widget*/);

    ToolsWidget* toolsWidget = new ToolsWidget();

    //创建一个QSplitter类，用于分隔两个Widget
    qsplitter->setOrientation(Qt::Horizontal);

    //添加Widget
    qsplitter->addWidget(toolsWidget);
    qsplitter->addWidget(runWidget);

    //设置比例
    qsplitter->setSizes(QList<int> {this->width() / 10, this->width() * 9 / 10});

    setCentralWidget(qsplitter);

//    resizeEvent(nullptr);
//    this->setCentralWidget(runWidget);
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
//    runWidget->setGeometry(0, 0, this->width(), this->height());
//    resizeWidget();

}


