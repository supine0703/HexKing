#ifndef GAMERECORD_H
#define GAMERECORD_H

#include <QVector>

using GCell = QPair<int, int>;

class QTextStream;


class GameRecord
{
public:
    static bool Mkdir();
    static bool SaveGameRecord(const QVector<GCell>& gameRecord, int winner);

private:
    GameRecord();
    class GameSave
    {
    public:
        virtual ~GameSave() { }
        virtual void SavaRecord(const QVector<GCell>& gameRecord) = 0;
    };

    class HexSave : public GameSave
    {
    public:
        HexSave(QTextStream& out) : out(out) { }
        void SavaRecord(const QVector<GCell> &gameRecord) override;
    private:
        QTextStream& out;
    };


    static QString dirPath;
    static GameSave* gs;
};

#endif // GAMERECORD_H
