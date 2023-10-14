#include "MainWindow.h"

#include "LogWidget.h"
#include "ChessBoard.h"
#include "FuncWidget.h"
#include "StartWidget.h"


#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QApplication>
#include <QDockWidget>
#include <QTextEdit>
#include <QScrollBar>

#include <HexLog.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow {parent}
{
    this->setWindowIconText("HexKing");
    this->setGeometry(550, 200, 800, 650);
    this->setStyleSheet("font:JetBrains Mono NL");
    this->setCentralWidget(new StartWidget(this));

    auto mainMenu = new QAction("Main Menu", this);
    auto restart = new QAction("Restart", this);
    auto exit = new QAction("Exit", this);

    funcView = new QAction("FuncView", this);
    funcView->setCheckable(true);
    logView = new QAction("LogView", this);
    logView->setCheckable(true);

    auto file = this->menuBar()->addMenu("&File");
    auto view = this->menuBar()->addMenu("&VieW");
//    auto help = this->menuBar()->addMenu("&Help");


    funcDock = new QDockWidget("FuncView", this);
    funcDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    funcDock->setWidget(new FuncWidget(funcDock));

    LogWidget* logWidget;
    logDock = new QDockWidget("LogView", this);
    logDock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    logDock->setWidget(logWidget = new LogWidget(logDock));
    logTxt = logWidget->LogTxt();

    file->addAction(mainMenu);
    file->addAction(restart);
    file->addSeparator();
    file->addAction(exit);

    view->addAction(funcView);
    view->addAction(logView);

    this->addDockWidget(Qt::LeftDockWidgetArea, funcDock);
    this->addDockWidget(Qt::BottomDockWidgetArea, logDock);

    this->menuBar()->setVisible(false);
    funcDock->setHidden(true);
    logDock->setHidden(true);

    connect(
        static_cast<StartWidget*>(this->centralWidget()),
        &StartWidget::start_game, this, &MainWindow::InitChess
    );
    connect(mainMenu, &QAction::triggered, this, [&]() {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Return main menu", "Are you sure you want to return main menu ?",
            QMessageBox::Yes | QMessageBox::No
        );
        if (reply == QMessageBox::Yes) {
            funcView->setChecked(false);
            funcDock->setHidden(true);
            logView->setChecked(false);
            logDock->setHidden(true);
            this->menuBar()->setVisible(false);
            this->setCentralWidget(new StartWidget(this));
            logTxt->clear();
            connect(
                static_cast<StartWidget*>(this->centralWidget()),
                &StartWidget::start_game, this, &MainWindow::InitChess
            );
        }
    });
    connect(restart, &QAction::triggered, this, [&]() {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Restart", "Are you sure you want to restart ?",
            QMessageBox::Yes | QMessageBox::No
        );
        if (reply == QMessageBox::Yes) {
            this->setCentralWidget(new ChessBoard(this->order, this->first, this->gmd, this));
            logTxt->clear();
        }
    });
    connect(exit, &QAction::triggered, this, [&]() {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Exit", "Are you sure you want to exit ?",
            QMessageBox::Yes | QMessageBox::No
        );
        if (reply == QMessageBox::Yes) {
            qApp->quit();
        }
    });
    connect(funcView, &QAction::toggled, this, [&](auto visible) {
        if (visible) {
            funcDock->setHidden(false);
        } else {
            funcDock->setHidden(true);
        }
    });
    connect(funcDock, &QDockWidget::visibilityChanged, this, [&](auto visible) {
        if (!visible) {
            funcView->setChecked(false);
        } else {
            funcView->setChecked(true);
        }
    });
    connect(logView, &QAction::toggled, this, [&](auto visible) {
        if (visible) {
            logDock->setHidden(false);
        } else {
            logDock->setHidden(true);
        }
    });
    connect(logDock, &QDockWidget::visibilityChanged, this, [&](auto visible) {
        if (!visible) {
            logView->setChecked(false);
        } else {
            logView->setChecked(true);
        }
    });
    connect(&hexLog, &HexLog::txt, this, [&](auto str, auto fmt) {
        QScrollBar *bar = logTxt->verticalScrollBar();
        int bottom = bar->maximum();
        logTxt->textCursor().insertText(str, fmt);
        if (bar->value() == bottom)
        {
            bar->setValue(bar->maximum());
        }
    });
    connect(&hexLog, &HexLog::plyer, logWidget, &LogWidget::SetStatePlayer);
    connect(&hexLog, &HexLog::ai, logWidget, &LogWidget::SetStateAI);
    connect(&hexLog, &HexLog::warning, logWidget, &LogWidget::SetStateWarn);
}

void MainWindow::InitChess(int order, int first, int gmd)
{
    this->order = order;
    this->first = first;
    this->gmd = gmd;
    this->setCentralWidget(new ChessBoard(this->order, this->first, this->gmd, this));
    this->menuBar()->setHidden(false);
    if (gmd != 3)
    {
        funcView->setEnabled(false);
        return;
    }
    funcView->setEnabled(true);
    funcView->setChecked(true);
    funcDock->setHidden(false);
    logView->setChecked(true);
    logDock->setHidden(false);
    auto chess = this->centralWidget();
    connect(
        static_cast<FuncWidget*>(funcDock->widget()), &FuncWidget::AIMove,
        static_cast<ChessBoard*>(chess), &ChessBoard::ai_move
    );
    connect(
        static_cast<FuncWidget*>(funcDock->widget()), &FuncWidget::AIStop,
        static_cast<ChessBoard*>(chess), &ChessBoard::ai_stop
    );
    connect(
        static_cast<FuncWidget*>(funcDock->widget()), &FuncWidget::AIDemo,
        static_cast<ChessBoard*>(chess), &ChessBoard::ai_demo
    );
    connect(
        static_cast<FuncWidget*>(funcDock->widget()), &FuncWidget::DemoStop,
        static_cast<ChessBoard*>(chess), &ChessBoard::demo_stop
    );
    connect(
        static_cast<FuncWidget*>(funcDock->widget()), &FuncWidget::RegretAMove,
        static_cast<ChessBoard*>(chess), &ChessBoard::regret_a_move
    );
}




