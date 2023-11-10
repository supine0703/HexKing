#ifndef PYINTERFACE_HPP
#define PYINTERFACE_HPP

#include "HexLocation.hpp"
#include "HexMatrix.hpp"
#include <QVector>

#include <math.h>
#undef slots
#include <Python.h>


static PyObject *pModule = nullptr;
static PyObject *pClass = nullptr;
static PyObject *pFunc = nullptr;

inline QVector<double> BoardToNP(
    const HexMatrix& state,
    HexLocation move1,
    HexLocation move2,
    HexAttacker attacker)
{
    QVector<double> values(247, 0);

    // 持方 前一次移动 前两次移动
    PyObject* list1 = PyList_New(3);
    PyList_SetItem(list1, 0, PyBool_FromLong(*attacker));
    PyList_SetItem(
        list1, 1, PyTuple_Pack(2, PyLong_FromLong(move1.row), PyLong_FromLong(move1.col))
    );
    PyList_SetItem(
        list1, 2, PyTuple_Pack(2, PyLong_FromLong(move2.row), PyLong_FromLong(move2.col))
    );

    // 棋盘状态
    PyObject* list2 = PyList_New(11);
    for (int i = 0; i < 11; i++) {
        PyObject* row = PyList_New(11);

        for (int j = 0; j < 11; j++) {
            PyList_SetItem(row, j, PyLong_FromLong(static_cast<int>(state(i, j))));
        }

        PyList_SetItem(list2, i, row);
    }

    PyObject* args = PyTuple_Pack(2, list1, list2);

    PyObject* result = PyObject_CallObject(pFunc, args);
    Q_ASSERT(result);
    Q_ASSERT(PyList_Check(result));

//    return values;

    int size = PyList_Size(result);
    Q_ASSERT(size == 247);
    for(int i = 0; i < size; i++)
    {
        PyObject* item = PyList_GetItem(result, i);
        values[i] = PyFloat_AsDouble(item);
    }

    Py_DECREF(result);

    return values;
}

inline void model_init()
{
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('" PY_PATH "')");
    PyRun_SimpleString("sys.path.append('E:/Tools/anaconda3/envs/py36/lib/site-packages')");
    pModule = PyImport_ImportModule("hex_nn");
    Q_ASSERT(pModule);
    pClass = PyObject_GetAttrString(pModule, "ModelTest");
    Q_ASSERT(pClass);
    PyObject* pArgs = PyTuple_New(0);
    PyObject* pInstance = PyObject_CallObject(pClass, pArgs);
    Q_ASSERT(pInstance);
    pFunc = PyObject_GetAttrString(pInstance, "test_state");
    Q_ASSERT(pFunc);
}

inline void model_closes()
{
    pFunc = nullptr;
    pClass = nullptr;
    pModule = nullptr;
    Py_Finalize();
}


#endif // PYINTERFACE_HPP
