#include "GameRecord.h"

#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QTextStream>

QString GameRecord::dirPath = "";
GameRecord::GameSave* GameRecord::gs = nullptr;

GameRecord::GameRecord()
{

}


bool GameRecord::Mkdir()
{
    dirPath = QDate::currentDate().toString("yyyy-MM-dd");

    QDir dir(RECORDS_HOME);

    if (!dir.exists(dirPath))
    {
        if (dir.mkdir(dirPath)) {
            qDebug() << "创建文件夹: " << dirPath;
        } else {
            qWarning() << "无法创建文件夹: " << dirPath;
            return false;
        }
    } else {
        qDebug() << "文件夹已存在: " << dirPath;
    }
    dirPath = QString(RECORDS_HOME) + "/" + dirPath;
    return true;
}

bool GameRecord::SaveGameRecord(const QVector<GCell> &gameRecord, int winner)
{
    QDir dir(dirPath);
    if (!dir.exists())
    {
        qDebug() << "目录不存在" + dirPath + "，请先调用 'GameRecord::Mkdir' 创建 ";
        return false;
    }

    QString txt[4], gameType;
    int gmType = 0, flag = 0;

    QFile read(RECORDS_HOME "/.setting.txt");
    if (read.open(QIODevice::ReadOnly))
    {
        QTextStream in(&read);
        QString line = in.readLine();
        auto data = line.split("##");
        read.close();
        flag = data[1].toInt();
        if (flag)
        {
            for (int i = 0; i < 4; i++)
            {
                txt[i] = data[i+3];
            }
        }
        gmType = data[2].toInt();
    }
    else
    {
        qDebug() << "not found record's .setting file";
        QFile read2(RECORDS_HOME "/.template.txt");
        if (!read2.open(QIODevice::ReadOnly))
        {
            qWarning() << "error: can not found record's .template file!";
            Q_ASSERT(false);
        }
        QTextStream in(&read);
        QString line = in.readLine();
        auto data = line.split("##");
        read.close();
        for (int i = 0; i < 4; i++)
        {
            txt[i] = data[i];
        }
        gmType = data[4].toInt();
    }

    switch (gmType)
    {
    case 0:
        gameType = "HEX";
        break;
    }

    QString fileName;

    if (flag)
    {
        fileName = (
            gameType + "-" + txt[2] + " R vs " + txt[3] + " B-"
            +(winner ? "后" : "先") + "手胜"
            + QDateTime::currentDateTime().toString("-yyyy-MM-dd_hh-mm-ss ")
            + txt[1] + "-" + txt[0] + ".txt"
        );
    }
    else
    {
        fileName = gameType
                   + QDateTime::currentDateTime().toString("-yyyy-MM-dd_hh-mm-ss")
                   + ".txt";
    }

    QString filePath = dirPath + "/" + fileName;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "文件创建失败";
        return false;
    }
    qDebug() << "成功创建文件: " << filePath;

    QTextStream stream(&file);
    switch (gmType)
    {
    case 0:
        gs = new GameRecord::HexSave(stream);
        break;
    }

    if (flag)
    {
        stream << "{[" << gameType << "][" << txt[2] << " R][" << txt[3]
               << " B][" << (winner ? "后" : "先") << "手胜]["
               << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm ")
               << txt[1] << "][" << txt[0] << "];";
    }
    else
    {
        stream << "{[" << (winner ? "Blue 后" : "Red 先") << "手胜];";
    }
    gs->SavaRecord(gameRecord);
    stream << "}";

    qDebug() << "对局记录已存储完毕.";
    delete gs;
    file.close();
    return true;
}

void GameRecord::HexSave::SavaRecord(const QVector<GCell> &gameRecord)
{
//    out << "{[HEX][<先手参赛队> R][<后手参赛队> B]["
//        << (winner ? "后" : "先") << "手胜]["
//        << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm")
//        << " 安徽][2023 安徽省计算机博弈大赛];";
    bool attacker = 0;
    char color[] = { 'R', 'B' };
    for (auto point : gameRecord)
    {
        auto [x, y] = point;
        out << color[attacker] << QString("(%1,%2);").arg(QChar('A' + x)).arg(y);
    }
}
