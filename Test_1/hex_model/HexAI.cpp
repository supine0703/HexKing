#include "HexAI.hpp"

#include "AI_Mcts_A.h"
#include "AI_Mcts_E.h"
#include "AI_Mcts_G.h"
#include "AI_Mcts_I.h"
#include "Py_Mcts.hpp"
//#include "SgException.hpp"

#include <QFile>
#include <QTextStream>

void ChooseAI::choose_ai(int key, HexAI*& black, HexAI*& white)
{
    Q_ASSERT(key);
    QFile file(CONFIG_FILE);
    if (!file.open(QIODevice::ReadOnly))
    {
        Q_ASSERT(false);
    }
    QTextStream in(&file);
    QString line = in.readLine();
    auto data = line.split(" ");
    file.close();

    auto _createAI = [](auto version, auto ecf, auto time, auto parallelized)
    {
        HexAI* ai(nullptr);
        switch (version)
        {
        case 0:
            ai = new AI_Mcts_A(ecf, time, parallelized);
            break;
        case 1:
            ai = new AI_Mcts_E(ecf, time, parallelized);
            break;
        case 2:
            ai = new AI_Mcts_G(ecf, time, parallelized);
            break;
        case 3:
//            ai = new AI_Mo_I(time);
            ai = new AI_Mcts_I(ecf, time, parallelized);
            break;
        case 4:
            ai = new PyMcts(ecf, time, parallelized);
        }
        if (!ai)
        {
//            throw SgException("AI Version not Found !");
        }
        return ai;
    };

    if (key & 1)
    {
        black = _createAI(data[3].toInt(), data[4].toDouble(), data[5].toInt(), data[6].toInt());
    }
    if ((key >> 1) & 1)
    {
        white = _createAI(data[7].toInt(), data[8].toDouble(), data[9].toInt(), data[10].toInt());
    }
}
