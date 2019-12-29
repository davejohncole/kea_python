#include "keamodule.h"

using namespace std;
using namespace isc::hooks;

extern "C" {

static PyMethodDef CalloutManager_methods[] = {
    {0}  // Sentinel
};

static PyObject *
CalloutManager_use_count(CalloutManagerObject *self, void *closure) {
    return (PyLong_FromLong(self->manager.use_count()));
}

static PyGetSetDef CalloutManager_getsetters[] = {
    {(char *)"use_count", (getter) CalloutManager_use_count, (setter) 0, (char *)"shared_ptr use count", 0},
    {0}  // Sentinel
};

static void
CalloutManager_dealloc(CalloutManagerObject *self) {
    self->manager.reset();
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static int
CalloutManager_init(CalloutManagerObject *self, PyObject *args, PyObject *kwds) {
    static const char *kwlist[] = {0};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "", (char**)kwlist)) {
        return (-1);
    }

    try {
        self->manager.reset(new CalloutManager());
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (-1);
    }

    return (0);
}

static PyObject *
CalloutManager_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    CalloutManagerObject *self;
    self = (CalloutManagerObject *) type->tp_alloc(type, 0);
    if (self) {
        self->manager.reset();
    }
    return ((PyObject *) self);
}

static PyTypeObject CalloutManagerType = {
    PyObject_HEAD_INIT(0)
    "kea.CalloutManager",                       // tp_name
    sizeof(CalloutManagerObject),               // tp_basicsize
    0,                                          // tp_itemsize
    (destructor) CalloutManager_dealloc,        // tp_dealloc
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
    "Kea server CalloutManager",                // tp_doc
    0,                                          // tp_traverse
    0,                                          // tp_clear
    0,                                          // tp_richcompare
    0,                                          // tp_weaklistoffset
    0,                                          // tp_iter
    0,                                          // tp_iternext
    CalloutManager_methods,                     // tp_methods
    0,                                          // tp_members
    CalloutManager_getsetters,                  // tp_getset
    0,                                          // tp_base
    0,                                          // tp_dict
    0,                                          // tp_descr_get
    0,                                          // tp_descr_set
    0,                                          // tp_dictoffset
    (initproc) CalloutManager_init,             // tp_init
    PyType_GenericAlloc,                        // tp_alloc
    CalloutManager_new                          // tp_new
};

int
CalloutManager_Check(PyObject *object) {
    return (Py_TYPE(object) == &CalloutManagerType);
}

int
CalloutManager_define() {
    if (PyType_Ready(&CalloutManagerType) < 0) {
        return (1);
    }
    Py_INCREF(&CalloutManagerType);
    if (PyModule_AddObject(kea_module, "CalloutManager", (PyObject *) &CalloutManagerType) < 0) {
        Py_DECREF(&CalloutManagerType);
        return (1);
    }

    return (0);
}

}
