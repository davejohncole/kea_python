#include "keamodule.h"

using namespace std;
using namespace isc::hooks;

extern "C" {

static PyMethodDef CalloutManager_methods[] = {
    {0}  // Sentinel
};

static PyObject *
CalloutManager_use_count(CalloutManagerObject *self, void *closure) {
    // REFCOUNT: PyLong_FromLong - returns new reference
    return (PyLong_FromLong(self->manager.use_count()));
}

static PyGetSetDef CalloutManager_getsetters[] = {
    {(char *)"use_count", (getter) CalloutManager_use_count, (setter) 0, (char *)"shared_ptr use count", 0},
    {0}  // Sentinel
};

// tp_dealloc - called when refcount is zero
static void
CalloutManager_dealloc(CalloutManagerObject *self) {
    self->manager.~CalloutManagerPtr();
    Py_TYPE(self)->tp_free((PyObject *) self);
}

// tp_init - called after tp_new has returned an instance
static int
CalloutManager_init(CalloutManagerObject *self, PyObject *args, PyObject *kwds) {
    new(&self->manager) CalloutManagerPtr;

    if (kwds != 0) {
        PyErr_SetString(PyExc_TypeError, "keyword arguments are not supported");
        return (-1);
    }
    if (!PyArg_ParseTuple(args, "")) {
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

// tp_new - allocate space and initialisation that can be repeated
static PyObject *
CalloutManager_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    CalloutManagerObject *self;
    self = (CalloutManagerObject *) type->tp_alloc(type, 0);
    if (self) {
        new(&self->manager) CalloutManagerPtr;
    }
    return ((PyObject *) self);
}

PyTypeObject CalloutManagerType = {
    .ob_base = PyObject_HEAD_INIT(0)
    .tp_name = "kea.CalloutManager",
    .tp_basicsize = sizeof(CalloutManagerObject),
    .tp_dealloc = (destructor) CalloutManager_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Kea server CalloutManager"),
    .tp_methods = CalloutManager_methods,
    .tp_getset = CalloutManager_getsetters,
    .tp_init = (initproc) CalloutManager_init,
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = CalloutManager_new,
};

int
CalloutManager_define() {
    // PyType_Ready - finish type initialisation
    if (PyType_Ready(&CalloutManagerType) < 0) {
        return (1);
    }
    Py_INCREF(&CalloutManagerType);
    // REFCOUNT: PyModule_AddObject - steals reference on success
    if (PyModule_AddObject(kea_module, "CalloutManager", (PyObject *) &CalloutManagerType) < 0) {
        Py_DECREF(&CalloutManagerType);
        return (1);
    }

    return (0);
}

}
