#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MenuWidget; }
QT_END_NAMESPACE
class QSpacerItem;

class MenuWidget : public QWidget
{
    Q_OBJECT

signals:
    void quit();

private slots:
    void on_Button_true_clicked();
    void on_Button_false_clicked();
    void on_Button_quit_clicked();
    void on_Button_start_clicked();
    void on_Button_options_clicked();
    void on_Button_more_clicked();

public:
    MenuWidget(QWidget *parent = nullptr);
    ~MenuWidget();

private:
    Ui::MenuWidget *ui;

    const QPixmap *bgPixmap;
    const QPixmap bg_mainMenu;
    const QPixmap bg_otherMenu;

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

public:
    bool eventFilter(QObject *watched, QEvent *event) override;

};

#endif // MENUWIDGET_H
