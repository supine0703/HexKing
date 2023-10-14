#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>

namespace Ui {
class LogWidget;
}

class QTextEdit;

class LogWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LogWidget(QWidget *parent = nullptr);
    ~LogWidget();

    void SetStatePlayer();
    void SetStateAI(bool attacker, QString owner = "AI");
    void SetStateWarn();
    QTextEdit* LogTxt();

private:
    Ui::LogWidget *ui;
    QString state1;
    QString state2;
    bool atc;
    bool warn;
    static QString stateColor[3];
};

#endif // LOGWIDGET_H
