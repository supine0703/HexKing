#ifndef STARTWIDGET_H
#define STARTWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class StartWidget; }
QT_END_NAMESPACE

class StartWidget : public QWidget
{
    Q_OBJECT

public:
    StartWidget(QWidget *parent = nullptr);
    ~StartWidget();

private:
    Ui::StartWidget *ui;

signals:
    void start_game(int order, int first, int gmd);

private slots:
    void on_startButton_clicked();
    void on_modeComboBox_currentIndexChanged(int index);
    void on_WhiteRadio_clicked();
    void on_BlackRadio_clicked();
};

#endif // STARTWIDGET_H
