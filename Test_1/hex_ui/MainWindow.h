#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QTextEdit;
class HexDock;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:

    void InitChess(int order, int first, int gmd);

    QAction* funcView;
    QAction* logView;

    HexDock* funcDock;
    HexDock* logDock;
    QTextEdit* logTxt;

    bool fcViewCp;
    bool first;
    int order;
    int gmd;

private slots:
    void SaveHexGame(bool winner);
};

#endif // MAINWINDOW_H
