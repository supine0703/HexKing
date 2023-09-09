#include "MenuWidget.h"
#include "./ui_MenuWidget.h"
#include "configurations"

#include <QRegularExpression>
#include <QPainter>
#include <QLayout>
#include <QList>

MenuWidget::MenuWidget(QWidget *parent)
    : QWidget{parent}
    , ui(new Ui::MenuWidget)
    , bgPixmap(&bg_mainMenu)
    , bg_mainMenu(QPixmap(BG_MAIN_MENU))
    , bg_otherMenu(QPixmap(BG_OTHER_MENU))
{
    ui->setupUi(this);
    ui->QuitWidget->setHidden(true);
    this->setGeometry(0, 0, WIDTH_START, HEIGHT_START);
    ui->MainWidget->setGeometry(RECT_MAIN_MENU(this->width(), this->height()));
    ui->Button_start->setFocus();

    QList<QPushButton*> buttons = this->findChildren<QPushButton*>();
    for(QPushButton* button : buttons)
    {
        button->installEventFilter(this);
    }
}

MenuWidget::~MenuWidget()
{
    bgPixmap = nullptr;
    delete ui;
}

void MenuWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    if (!bgPixmap || bgPixmap->isNull())
    {
        painter.drawPixmap(rect(), QPixmap());
    }
    else
    {
        painter.drawPixmap(rect(), *bgPixmap);
    }
    update();
//    if (!ui->QuitWidget->isHidden())
//    {
//        painter.fillRect(rect(), QColor(0, 0, 0, 127));
//        update();
//    }
}

void MenuWidget::resizeEvent(QResizeEvent* event)
{
    // set geometry of mainMenu
    int bgWidth = this->width();
    int bgHeight = this->height();
    if (!ui->QuitWidget->isHidden())
    {
        ui->QuitWidget->move(POINT_QUIT_MENU(bgWidth, bgHeight));
    }
    if (!ui->MainWidget->isHidden())
    {
        ui->MainWidget->setGeometry(RECT_MAIN_MENU(bgWidth, bgHeight));
    }

    // define regular expression and String for replacing
    static QRegularExpression re_1(
        R"(QPushButton:focus\s*\{\s*font:\s*\d+(\.\d+)?pt)"
    );
    static QString newRule_1 = QString(
        "QPushButton:focus\n{\n  font: %1pt"
    );
    static QRegularExpression re_2(
        R"(QPushButton\s*\{\s*min-height:\s*\d+(\.\d+)?px;\s*font:\s*\d+(\.\d+)?pt)"
    );
    static QString newRule_2 = QString(
        "QPushButton\n{\n  min-height: %1px;\n  font: %2pt"
    );

    // set front-size of button
    QString styleSheet = this->styleSheet();
    float size;
    QString newRule = newRule_1.arg((size = qMin(bgWidth, bgHeight)) / 20, 0, 'f', 2);
    styleSheet.replace(re_1, newRule);
    newRule = newRule_2.arg(size / 14, 0, 'f', 2).arg(size / 25, 0, 'f', 2);
    styleSheet.replace(re_2, newRule);
    this->setStyleSheet(styleSheet);
//    update();
}

bool MenuWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::HoverEnter)
    {
        auto button = static_cast<QPushButton*>(obj);
        button->setFocus();
        return true;
    }
    return QWidget::eventFilter(obj, event);
}


void MenuWidget::on_Button_true_clicked()
{
    emit quit();
}

void MenuWidget::on_Button_false_clicked()
{
    bgPixmap = &bg_mainMenu;
    ui->QuitWidget->setHidden(true);
    ui->MainWidget->setHidden(false);
    ui->MainWidget->setGeometry(RECT_MAIN_MENU(this->width(), this->height()));
    ui->Button_start->setFocus();
}

void MenuWidget::on_Button_quit_clicked()
{
    bgPixmap = &bg_otherMenu;
    ui->MainWidget->setHidden(true);
    ui->QuitWidget->setHidden(false);
    ui->QuitWidget->move(POINT_QUIT_MENU(this->width(), this->height()));
    ui->Button_false->setFocus();
}

void MenuWidget::on_Button_start_clicked()
{

}

void MenuWidget::on_Button_options_clicked()
{

}

void MenuWidget::on_Button_more_clicked()
{

}

