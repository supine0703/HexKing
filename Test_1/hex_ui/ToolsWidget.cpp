#include "ToolsWidget.h"
#include "./ui_ToolsWidget.h"

ToolsWidget::ToolsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ToolsWidget)
{
    ui->setupUi(this);
    connect(ui->AImove, &QPushButton::clicked, this, [=](){
        emit AIMove();
    });
}

ToolsWidget::~ToolsWidget()
{
    delete ui;
}


