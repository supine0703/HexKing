#include "Py_Mcts.hpp"
#include "MoWork.h"

#include <QElapsedTimer>
#include <QVector>
#include "HexLog.h"

#include "PyThreadStateLock.hpp"
static PyObject* pyAction = nullptr;
#include "ShareData.h"

PyMcts::PyMcts(bool useMo)
    : useMo(useMo)
{
    if (useMo)
    {
        mow = new MoWork(5000);
    }
}

PyMcts::~PyMcts()
{

    PyThreadStateLock PyThreadLock;
    pyAction = nullptr;
    Py_Finalize();
}

HexLocation PyMcts::ChooseMove(const HexMatrix &board, HexAttacker attacker)
{

//    class PyThreadStateLock PyThreadLock;
    static bool needInit = true;
    if (needInit)
    {
        init();
        needInit = false;
    }

//    qDebug() << pyAction;

    usedTime = new QElapsedTimer();
    usedTime->start();
    if (useMo)
    {
        QVector<HexLocation> moves;
        mow->Work(usedTime, board, attacker, moves);
        hexLog() << "VCs used time:" << usedTime->elapsed()
                 << (attacker == HexAttacker::Black ? hlg::bdl : hlg::wdl);
        if (moves.count() == 1)
        {
            delete usedTime;
            usedTime = nullptr;
            return moves[0];
        }
    }

    int end = qMin(his().size(), hisMove().size());
    for (int i = 0; i < end; i++)
    {
        if (his()[i] != hisMove()[i])
        {
            end = i;
            break;
        }
    }


    PyObject* pArgs = PyTuple_New(1);
    if (end < his().size())
    {
        PyTuple_SetItem(pArgs, 0, Py_BuildValue("i", end - his().size()));
        PyObject_CallObject(pyAction, pArgs);
    }

    while (end < hisMove().size())
    {
        if (his().size() > end)
        {
            his()[end] = hisMove()[end];
        }
        else
        {
            his().push_back(hisMove()[end]);
        }
        PyTuple_SetItem(pArgs, 0, Py_BuildValue("i", hisMove()[end]+1));
        PyObject_CallObject(pyAction, pArgs);
        end++;
    }
    if (end < his().size())
        his().remove(end, his().size() - end);
    Q_ASSERT(his().size() == hisMove().size());

    PyTuple_SetItem(pArgs, 0, Py_BuildValue("i", 0));
    Q_ASSERT(pyAction);
    PyObject* res = PyObject_CallObject(pyAction, pArgs);
    int result;
    PyArg_Parse(res, "i", &result);
    his().push_back(result);
    result--;
    int r = result/11, c = result%11;

//    qDebug() << result;

    int64_t usedT = usedTime->elapsed();

    hexLog() << "used time:" << usedT
             << "| total time:" << (totalTime+=usedT)
             << (attacker == HexAttacker::Black ? hlg::bdl : hlg::wdl);

    delete usedTime;
    usedTime = nullptr;

//    Py_DECREF(res);
//    qDebug() << pyAction;

    return HexLocation(r, c);
}

void PyMcts::init()
{
//    class PyThreadStateLock PyThreadLock;
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.insert(0,'" AZA_PATH "')");
    PyObject* pModule = PyImport_ImportModule("run_for_cpp");
    Q_ASSERT(pModule);
    PyObject* pClass = PyObject_GetAttrString(pModule, "CppRun");
    Q_ASSERT(pClass);
    PyObject* pArgs = PyTuple_New(1);
    PyTuple_SetItem(pArgs, 0, Py_BuildValue("s", AZA_PATH));
    PyObject* pInstance = PyObject_CallObject(pClass, pArgs);
    Q_ASSERT(pInstance);
    pyAction = PyObject_GetAttrString(pInstance, "action");
    Q_ASSERT(pyAction);
    qDebug() << "SSS";
}
