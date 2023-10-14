#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMenuBar>
#include <QMenu>
#include <QSplitter>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    void StartGame(int order, int first, int gmd);
    void AImove();

private:
    QSplitter* qsplitter;
    QWidget* runWidget;
    QWidget* toolsWidget;
    QWidget* Widget;
    QMenuBar* menuBar;
    QMenu* file;
    QMenu* options;
    QMenu* order;
    QMenu* mode;
    QMenu* function;

signals:
    void aimove();

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // MAINWINDOW_H
