#ifndef TOOLSWIDGET_H
#define TOOLSWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class ToolsWidget; }
QT_END_NAMESPACE

class ToolsWidget : public QWidget
{
    Q_OBJECT

public:
    ToolsWidget(QWidget *parent = nullptr);
    ~ToolsWidget();

signals:
    void Hint();
    void Demo();
    void AIMove();
    void WithDraw();

private:
    Ui::ToolsWidget *ui;

};
#endif // TOOLSWIDGET_H
