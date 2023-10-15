#ifndef HEXDOCK_H
#define HEXDOCK_H

#include <QDockWidget>

class HexDock : public QDockWidget
{
    Q_OBJECT
public:
    HexDock(QWidget* parent);
    HexDock(const QString& title, QWidget* parent);

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void clickClose();
};

#endif // HEXDOCK_H
