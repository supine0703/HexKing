#include "Py_Mcts.hpp"
#include "PMoWork.h"

#include <QElapsedTimer>
#include <QVector>
#include <QFile>
#include <QThread>
#include "HexLog.h"

#include "PyThreadStateLock.hpp"
PYTHREADSTATELOCK_H
static PyObject* pyAction = nullptr;
#include "ShareData.h"

PyMcts::PyMcts(double multiple, int time, bool useMo)
    : HexAI()
    , itNum(80000 * multiple)
    , gTime(time * 60000)
    , useMo(useMo)
{
    if (useMo)
    {
        mow = new PMoWork(runTime);
    }
}

PyMcts::~PyMcts()
{
    if (useMo)
    {
        delete mow;
    }
    //    isExit = true;
}

void PyMcts::StopWork()
{
    QFile file(PY_WORK_TXT);
    // python will crash if delete file too fast so that search no finish
    if (file.exists())
    {
        QThread::msleep(10);
        if (file.exists()) file.remove();
    }
    else
    {
        while (!file.exists() && usedTime && usedTime->elapsed() < 5000)
            QThread::msleep(100);
        if (file.exists())
        {
            QThread::msleep(500);
            if (file.exists()) file.remove();
        }
    }
}

static bool needInit = true;

void PyMcts::Exit()
{
    if (!needInit)
    {
        Py_Finalize();
        needInit = true;
    }
    //    qDebug() << "End";
}

void PyMcts::Init()
{
    if (needInit)
    {
        init();
        needInit = false;
    }
}

class TimeThread : public QThread
{
public:
    TimeThread(QElapsedTimer& usedTime, int time)
        : usedTime(usedTime)
        , time(time) { }
    void run() override
    {
        while (time - usedTime.elapsed() - 10 > 0 && !stop)
            QThread::msleep(200);
        QFile file(PY_WORK_TXT);
        if (file.exists())
        {
            QThread::msleep(10);
            if (file.exists()) file.remove();
            qDebug() << "time over";
        }
    }
    void Stop() { stop = true; }
private:
    QElapsedTimer& usedTime;
    int time;
    bool stop = false;
};

HexLocation PyMcts::ChooseMove(const HexMatrix &board, HexAttacker attacker)
{
    usedTime = new QElapsedTimer();
    usedTime->start();
    double multiple = 1;
    if (useMo && hisMove().size() > 12)
    {
        auto move = mow->Work(usedTime, board, attacker);
        hexLog() << "VCs used time:" << usedTime->elapsed();
        if (move.row == 199)
        {
            multiple = 1.5;
            hexLog() << (attacker == HexAttacker::Black ? hlg::bdl : hlg::wdl)
                     << "will lose, double number of iterations"
                     << (attacker == HexAttacker::Black ? hlg::bdl : hlg::wdl);
        }
        else if (move.row != 100)
        {
            int64_t usedT = usedTime->elapsed();
            hexLog() << "| total time:" << (totalTime+=usedT)
                     << (attacker == HexAttacker::Black ? hlg::bdl : hlg::wdl);
            delete usedTime;
            usedTime = nullptr;
            return move;
        }
        else hexLog() << (attacker == HexAttacker::Black ? hlg::bdl : hlg::wdl);
    }

    TimeThread* tt = new TimeThread(
        *usedTime,
        (multiple>1 ? 2:1) * (gTime-totalTime) / 25);
    if (gTime - totalTime - usedTime->elapsed() > 60000)
    {
        tt->start();
    }
    else
    {
        qDebug() << "time so little, start quick";
        itNum = 800;
        multiple = 1;
    }

    // if regret moves, end != his().size()
    // or end != hisMove().size() or his().size() != hisMove().size()
    int end = qMin(his().size(), hisMove().size());
    for (int i = 0; i < end; i++)
    {
        if (his()[i] != hisMove()[i])
        {
            end = i;
            break;
        }
    }

    // regret moves
    PyObject* pArgs = PyTuple_New(1);
    if (end < his().size())
    {
        PyTuple_SetItem(pArgs, 0, Py_BuildValue("i", end - his().size()));
        PyObject_CallObject(pyAction, pArgs);
        qDebug() << "i";
    }

    // synchronize the game
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
    // synchronize the history of moves
    if (end < his().size())
        his().remove(end, his().size() - end);
    Q_ASSERT(his().size() == hisMove().size());


    Q_ASSERT(pyAction);
    // update number of iterations
    Q_ASSERT(itNum >= 800);
    PyTuple_SetItem(pArgs, 0, Py_BuildValue("i", static_cast<int>(itNum * multiple)));
    PyObject_CallObject(pyAction, pArgs);

    // choose a move
    PyTuple_SetItem(pArgs, 0, Py_BuildValue("i", 0));
    PyObject* res = PyObject_CallObject(pyAction, pArgs);
    int result;
    PyArg_Parse(res, "i", &result);
    result--;
    his().push_back(result);
    int r = result / 11;
    int c = result % 11;

//    qDebug() << result;

    int64_t usedT = usedTime->elapsed();

    hexLog() << "used time:" << usedT
             << "| total time:" << (totalTime+=usedT)
             << (attacker == HexAttacker::Black ? hlg::bdl : hlg::wdl);

    tt->Stop();
    tt->quit();
    tt->wait();
    tt->deleteLater();
    delete usedTime;
    usedTime = nullptr;

//    Py_DECREF(res);
//    qDebug() << pyAction;

    if (isExit)
    {
        qDebug() << "2";
        Exit();
        isExit = false;
    }

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
//    qDebug() << "SSS";
}
