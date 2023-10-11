#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMenuBar>
#include <QMenu>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    void StartGame(int order, int first, int gmd);

private:

    QWidget* runWidget;

    QMenuBar* menuBar;
    QMenu* file;
    QMenu* options;
    QMenu* order;
    QMenu* mode;
    QMenu* function;




signals:

};

#endif // MAINWINDOW_H
