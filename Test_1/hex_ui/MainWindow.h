#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QTextEdit;
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

    QDockWidget* funcDock;
    QDockWidget* logDock;
    QTextEdit* logTxt;

    int order;
    bool first;
    int gmd;
signals:

};

#endif // MAINWINDOW_H
