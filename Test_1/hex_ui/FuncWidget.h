#ifndef FUNCWIDGET_H
#define FUNCWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {class FuncWidget;}
QT_END_NAMESPACE

class FuncWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FuncWidget(QWidget *parent = nullptr);
    ~FuncWidget();

    void ReQuickFunc();

private:
    Ui::FuncWidget *ui;

signals:
    void AIQuick(bool);
    void AIMove();
    void AIStop();
    void AIDemo();
    void DemoStop();
    void RegretAMove();
private slots:
    void on_ai_move_clicked();
    void on_ai_stop_clicked();
    void on_ai_demo_clicked();
    void on_demo_stop_clicked();
    void on_regret_a_move_clicked();
    void on_ai_quick_clicked(bool checked);
};

#endif // FUNCWIDGET_H
