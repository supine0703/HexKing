#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    GameWindow(QWidget *parent = nullptr);
    ~GameWindow();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    QWidget *runWidget;

};

#endif // GAMEWINDOW_H
