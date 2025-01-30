#include "keamodule.h"

using namespace std;
using namespace isc::log;

extern "C" {

static PyObject *
LoggerManager_static_init(LoggerManagerObject *self, PyObject *args) {
    const char *root;

    if (!PyArg_ParseTuple(args, "s", &root)) {
        return (0);
    }

    try {
        LoggerManager::init(root, Severity::DEFAULT, 0, "/dev/null");
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyMethodDef LoggerManager_methods[] = {
    {"init", (PyCFunction) LoggerManager_static_init, METH_VARARGS|METH_STATIC,
     "Run-Time Initialization Performs run-time initialization of the logger system."},
    {0}  // Sentinel
};

PyTypeObject LoggerManagerType = {
    .ob_base = PyObject_HEAD_INIT(0)
    .tp_name = "kea.LoggerManager",
    .tp_basicsize = sizeof(LoggerManagerObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Kea server LoggerManager"),
    .tp_methods = LoggerManager_methods,
};

int
LoggerManager_define() {
    // PyType_Ready - finish type initialisation
    if (PyType_Ready(&LoggerManagerType) < 0) {
        return (1);
    }
    Py_INCREF(&LoggerManagerType);
    // REFCOUNT: PyModule_AddObject steals reference on success
    if (PyModule_AddObject(kea_module, "LoggerManager", (PyObject *) &LoggerManagerType) < 0) {
        Py_DECREF(&LoggerManagerType);
        return (1);
    }

    return (0);
}

}
