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
        LoggerManager::init(root);
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

static void
LoggerManager_dealloc(LoggerManagerObject *self) {
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static int
LoggerManager_init(LoggerManagerObject *self, PyObject *args, PyObject *kwds) {
    PyErr_SetString(PyExc_RuntimeError, "cannot directly construct");
    return (-1);
}

PyTypeObject LoggerManagerType = {
    PyObject_HEAD_INIT(0)
    "kea.LoggerManager",                        // tp_name
    sizeof(LoggerManagerObject),                // tp_basicsize
    0,                                          // tp_itemsize
    (destructor) LoggerManager_dealloc,         // tp_dealloc
    0,                                          // tp_vectorcall_offset
    0,                                          // tp_getattr
    0,                                          // tp_setattr
    0,                                          // tp_as_async
    0,                                          // tp_repr
    0,                                          // tp_as_number
    0,                                          // tp_as_sequence
    0,                                          // tp_as_mapping
    0,                                          // tp_hash
    0,                                          // tp_call
    0,                                          // tp_str
    0,                                          // tp_getattro
    0,                                          // tp_setattro
    0,                                          // tp_as_buffer
    Py_TPFLAGS_DEFAULT,                         // tp_flags
    "Kea server LoggerManager",                 // tp_doc
    0,                                          // tp_traverse
    0,                                          // tp_clear
    0,                                          // tp_richcompare
    0,                                          // tp_weaklistoffset
    0,                                          // tp_iter
    0,                                          // tp_iternext
    LoggerManager_methods,                      // tp_methods
    0,                                          // tp_members
    0,                                          // tp_getset
    0,                                          // tp_base
    0,                                          // tp_dict
    0,                                          // tp_descr_get
    0,                                          // tp_descr_set
    0,                                          // tp_dictoffset
    (initproc) LoggerManager_init,              // tp_init
    PyType_GenericAlloc,                        // tp_alloc
    PyType_GenericNew                           // tp_new
};

int
LoggerManager_define() {
    if (PyType_Ready(&LoggerManagerType) < 0) {
        return (1);
    }
    Py_INCREF(&LoggerManagerType);
    if (PyModule_AddObject(kea_module, "LoggerManager", (PyObject *) &LoggerManagerType) < 0) {
        Py_DECREF(&LoggerManagerType);
        return (1);
    }

    return (0);
}

}
