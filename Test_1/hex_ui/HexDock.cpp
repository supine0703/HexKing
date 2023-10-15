#include "HexDock.h"

HexDock::HexDock(QWidget *parent) : QDockWidget(parent)
{ }

HexDock::HexDock(const QString &title, QWidget *parent) : QDockWidget(title, parent)
{ }

void HexDock::closeEvent(QCloseEvent *event)
{
    emit clickClose();
}
