#include "FuncWidget.h"
#include "ui_FuncWidget.h"

FuncWidget::FuncWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FuncWidget)
{
    ui->setupUi(this);
}

FuncWidget::~FuncWidget()
{
    delete ui;
}

void FuncWidget::ReQuickFunc()
{
    emit AIQuick(ui->ai_quick->isChecked());
}


void FuncWidget::on_ai_move_clicked()
{
    emit AIMove();
}


void FuncWidget::on_ai_stop_clicked()
{
    emit AIStop();
}


void FuncWidget::on_ai_demo_clicked()
{
    emit AIDemo();
}


void FuncWidget::on_demo_stop_clicked()
{
    emit DemoStop();
}


void FuncWidget::on_regret_a_move_clicked()
{
    emit RegretAMove();
}


void FuncWidget::on_ai_quick_clicked(bool checked)
{
    emit AIQuick(checked);
}

