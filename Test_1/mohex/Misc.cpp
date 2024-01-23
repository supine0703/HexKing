#include "Misc.hpp"

#include "SgException.hpp"

//----------------------------------------------------------------------------
void MiscUtil::WordToBytes(unsigned word, byte* out)
{
    for (int i = 0; i < 4; i++)
    {
        out[i] = static_cast<byte>(word & 0xff);
        word >>= 8;
    }
}

unsigned MiscUtil::BytesToWord(const byte* bytes)
{
    unsigned ret = 0;
    for (int i = 3; i >= 0; i--)
    {
        ret <<= 8;
        ret |= bytes[i];
    }
    return ret;
}

int MiscUtil::NumBytesToHoldBits(int bits)
{
    return (bits + 7) / 8;
}

#include "SgException.hpp"

//----------------------------------------------------------------------------

#ifndef ABS_TOP_SRCDIR
    #error ABS_TOP_SRCDIR not defined!
#endif
#ifndef DATADIR
    #error ABS_TOP_SRCDIR not defined!
#endif

//----------------------------------------------------------------------------
#if USE_QT_FILESYSTEM

#include <QDir>
#include <QApplication>
#include <fstream>

std::string MiscUtil::OpenFile(std::string n, std::ifstream& f)
{
    QString name(n.c_str());

    static QDir programDir = QDir(QCoreApplication::applicationDirPath());

    QString programPath = programDir.absoluteFilePath(name);
    if (QFile::exists(programPath)) {
        std::string file = programPath.toStdString();
        f.open(file);
        if (f.is_open())
        {
            return file;
        }
    }

    QString absPath = QString(ABS_TOP_SRCDIR) + "/share/" + name;
    if (QFile::exists(absPath)) {
        std::string file = absPath.toStdString();
        f.open(file);
        if (f.is_open())
        {
            return file;
        }
    }

    QString dataPath = QString(DATADIR) + "/" + name;
    if (QFile::exists(dataPath)) {
        std::string file = dataPath.toStdString();
        f.open(file);
        if (f.is_open())
        {
            return file;
        }
    }

    throw SgException(
        "Could not find '" + name.toStdString() + "'. Tried\n"
        + "\t" + programPath.toStdString() + "'\n"
        + "\t" + absPath.toStdString() + "'\n"
        + "\t" + dataPath.toStdString() + "'."
    );
}

//----------------------------------------------------------------------------
#elif

#include <fstream>
#include <filesystem>

/** Directory of executable.
    Can be set with MiscUtil::FindProgramDir() and is used in
    MiscUtil::OpenFile() */
std::filesystem::path programDir;

void MiscUtil::FindProgramDir(int argc, char* argv[])
{
    if (argc == 0 || argv == 0)
        return;
    //    programDir = path(argv[0]).branch_path();
    //    programDir = path(argv[0], native).branch_path();
    programDir = std::filesystem::path(argv[0]).parent_path();
}

std::string MiscUtil::OpenFile(std::string name, std::ifstream& f)
{
    std::filesystem::path programPath = programDir / name;
    if (std::filesystem::exists(programPath)) {
        f.open(programPath);
        if (f.is_open()) {
            return programPath.string();
        }
    }

    std::filesystem::path absPath =
        std::filesystem::path(ABS_TOP_SRCDIR) / "share" / name;
    if (std::filesystem::exists(absPath)) {
        f.open(absPath);
        if (f.is_open()) {
            return absPath.string();
        }
    }

    std::filesystem::path dataPath = std::filesystem::path(DATADIR) / name;
    if (std::filesystem::exists(dataPath)) {
        f.open(dataPath);
        if (f.is_open()) {
            return dataPath.string();
        }
    }
    throw SgException(
        "Could not find '"
        + name
        + "'. Tried \n"
        + "\t" + programPath.string() + "' and\n"
        + "\t" + absPath.string() + "' and\n"
        + "\t" + absPath.string() + "'."
    );
}

#endif
//----------------------------------------------------------------------------

