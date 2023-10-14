#include "LogWidget.h"
#include "ui_LogWidget.h"

#include <QTextCursor>

QString LogWidget::stateColor[3] = {
    "color:rgb(234, 67, 53);",
    "color:rgb(66, 133, 244);",
    "color:rgb(255, 225, 135);"
};

LogWidget::LogWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogWidget),
    state1("Player is operating"),
    state2("is working")
{
    ui->setupUi(this);
    ui->stateLabel->setStyleSheet("font: 600 italic 16pt \"JetBrains Mono\"");
}

LogWidget::~LogWidget()
{
    delete ui;
}

void LogWidget::SetStatePlayer()
{
    warn = false;
    ui->stateLabel->setText(state1);
    ui->stateLabel->setStyleSheet("font: 600 italic 16pt \"JetBrains Mono\";");
}

void LogWidget::SetStateAI(bool attacker, QString owner)
{
    warn = false;
    atc = attacker;
    ui->stateLabel->setText(owner + state2);
    ui->stateLabel->setStyleSheet("font: 600 italic 16pt \"JetBrains Mono\";\n"+stateColor[atc]);
}

void LogWidget::SetStateWarn()
{
    warn = !warn;
    ui->stateLabel->setStyleSheet(
        "font: 600 italic 16pt \"JetBrains Mono\";\n"+
        (warn ? stateColor[2] : stateColor[atc])
    );
}

QTextEdit* LogWidget::LogTxt()
{
    return ui->logTextEdit;
}
