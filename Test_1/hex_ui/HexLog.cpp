#include "HexLog.h"

HexLog::HexLog(QObject *parent)
    : QObject{parent}
{
    for (auto& fmt : fmts)
    {
        fmt.setFont(QFont("JetBrains Mono NL", 12));
    }
    fmts[0].setForeground(Qt::black);
    fmts[1].setForeground(QColor(234, 67, 53));
    fmts[2].setForeground(QColor(66, 133, 244));
}

HexLog::~HexLog()
{
}

HexLog &HexLog::getHexLog()
{
    static HexLog log;
    return log;
}

HexLog& HexLog::operator<<(const qint64 num)
{
    buff.append(QString::number(num) + " ");
    return *this;
}

HexLog& HexLog::operator<<(const quint64 num)
{
    buff.append(QString::number(num)+ " ");
    return *this;
}

HexLog& HexLog::operator<<(const qint32 num)
{
    buff.append(QString::number(num) + " ");
    return *this;
}

HexLog& HexLog::operator<<(const quint32 num)
{
    buff.append(QString::number(num)+ " ");
    return *this;
}

HexLog& HexLog::operator<<(const qint16 num)
{
    buff.append(QString::number(num) + " ");
    return *this;
}

HexLog& HexLog::operator<<(const quint16 num)
{
    buff.append(QString::number(num)+ " ");
    return *this;
}

HexLog& HexLog::operator<<(const float num)
{
    buff.append(QString::number(num)+ " ");
    return *this;
}

HexLog& HexLog::operator<<(const double num)
{
    buff.append(QString::number(num)+ " ");
    return *this;
}

HexLog& HexLog::operator<<(const char c)
{
    buff.append(c);
    buff.append(" ");
    return *this;
}

HexLog& HexLog::operator<<(const QChar c)
{
    buff.append(c);
    buff.append(" ");
    return *this;
}

HexLog& HexLog::operator<<(const char str[])
{
    buff.append(str);
    buff.append(" ");
    return *this;
}

HexLog& HexLog::operator<<(const std::string str)
{
    buff.append(str + " ");
    return *this;
}

HexLog& HexLog::operator<<(const QString str)
{
    buff.append(str + " ");
    return *this;
}

HexLog& HexLog::operator<<(hlg flag)
{
    int flg = static_cast<int>(flag);
    if (flag == hlg::ln)
    {
        emit txt("\n", fmts[0]);
        return *this;
    }
    if (flg & 1)
    {
        buff.chop(1);
        buff.append('\n');
    }
    emit txt(buff, fmts[flg >> 1]);
    buff.clear();
    return *this;
}

HexLog& HexLog::operator<<(hst flag)
{
    switch (flag)
    {
    case hst::blkai:
    case hst::whtai:
        emit ai(static_cast<bool>(flag), buff);
        buff.clear();
        break;
    case hst::plyer:
        emit plyer();
        break;
    case hst::aiwarn:
        emit warning();
        break;
    }
    return *this;
}
