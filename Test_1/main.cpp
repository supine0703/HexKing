#include "ChessBoard.h"
#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    ChessBoard w;
    MainWindow w;
    w.show();
    return a.exec();
}
