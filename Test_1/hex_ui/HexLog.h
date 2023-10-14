#ifndef HEXLOG_H
#define HEXLOG_H

#define hexLog HexLog::getHexLog()

#include <QObject>

#include <QTextFormat>

enum class hlg
{
    end = 0,
    endl,
    bd,
    bdl,
    wd,
    wdl,
    ln
};

enum class hst
{
    blkai = 0,
    whtai,
    plyer,
    aiwarn
};

class HexLog : public QObject
{
    Q_OBJECT
    HexLog(const HexLog&) = delete;
    HexLog& operator=(const HexLog&) = delete;
public:
    ~HexLog();

    static HexLog& getHexLog();
    HexLog& operator<<(const qint64 num);
    HexLog& operator<<(const quint64 num);
    HexLog& operator<<(const qint32 num);
    HexLog& operator<<(const quint32 num);
    HexLog& operator<<(const qint16 num);
    HexLog& operator<<(const quint16 num);
    HexLog& operator<<(const float num);
    HexLog& operator<<(const double num);
    HexLog& operator<<(const char c);
    HexLog& operator<<(const QChar c);
    HexLog& operator<<(const char str[]);
    HexLog& operator<<(const std::string str);
    HexLog& operator<<(const QString str);
    HexLog& operator<<(hlg flag);
    HexLog& operator<<(hst flag);

private:
    explicit HexLog(QObject *parent = nullptr);

    QString buff;
    QTextCharFormat fmts[3];

signals:
    void txt(QString, QTextCharFormat);
    void plyer();
    void ai(bool, QString);
    void warning();
};

#endif // HEXLOG_H
