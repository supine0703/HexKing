#include "HexAI.h"
#include <QString>

HexAI::HexAI(const QString& name) : name(new QString(name))
{
}

QString HexAI::GetName()
{
    return *name;
}

HexAI::~HexAI()
{
    delete name;
}
