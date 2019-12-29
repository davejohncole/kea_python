#include "keamodule.h"

using namespace std;
using namespace isc::hooks;
using namespace isc::dhcp;

extern "C" {

static PyObject *
CalloutHandle_getArgument(PyObject *self, PyObject *args) {
    char *name;

    if (!PyArg_ParseTuple(args, "s", &name)) {
        return (0);
    }

    if (strcmp(name, "lease4") == 0) {
        try {
            Lease4Ptr ptr;
            ((CalloutHandleObject *)self)->handle->getArgument(name, ptr);
            return (Lease4_from_handle(ptr));
        }
        catch (const exception &e) {
            PyErr_SetString(PyExc_TypeError, e.what());
            return (0);
        }
    }

    PyErr_SetString(PyExc_ValueError, "Unknown argument");
    return (0);
}

static PyObject *
CalloutHandle_setArgument(PyObject *self, PyObject *args) {
    char *name;
    PyObject *value;

    if (!PyArg_ParseTuple(args, "sO", &name, &value)) {
        return (0);
    }

    if (strcmp(name, "lease4") == 0) {
        if (!Lease4_Check(value)) {
            PyErr_SetString(PyExc_TypeError, "Expected Lease4 object");
            return (0);
        }
        try {
            ((CalloutHandleObject *)self)->handle->setArgument(name, ((Lease4Object *)value)->ptr);
            Py_RETURN_NONE;
        }
        catch (const exception &e) {
            PyErr_SetString(PyExc_TypeError, e.what());
            return (0);
        }
    }

    PyErr_SetString(PyExc_ValueError, "Unknown argument");
    return (0);
}

static PyMethodDef CalloutHandle_methods[] = {
    {"getArgument", (PyCFunction) CalloutHandle_getArgument, METH_VARARGS,
     "Gets the value of an argument."},
    {"setArgument", (PyCFunction) CalloutHandle_setArgument, METH_VARARGS,
     "Sets the value of an argument."},
    {0}  // Sentinel
};

static void
CalloutHandle_dealloc(CalloutHandleObject *self) {
    if (self->is_owner) {
        delete self->handle;
    }
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static int
CalloutHandle_init(CalloutHandleObject *self, PyObject *args, PyObject *kwds) {
    static const char *kwlist[] = {"manager", 0};
    PyObject *manager = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", (char**)kwlist, &manager)) {
        return (-1);
    }

    if (!CalloutManager_Check(manager)) {
        PyErr_SetString(PyExc_TypeError, "manager must be instance of CalloutManager");
        return (-1);
    }

    try {
        self->handle = new CalloutHandle(((CalloutManagerObject *)manager)->manager);
        self->is_owner = true;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (-1);
    }

    return (0);
}

static PyObject *
CalloutHandle_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    CalloutHandleObject *self;
    self = (CalloutHandleObject *) type->tp_alloc(type, 0);
    if (self) {
        self->handle = 0;
        self->is_owner = false;
    }
    return ((PyObject *) self);
}

static PyTypeObject CalloutHandleType = {
    PyObject_HEAD_INIT(0)
    "kea.CalloutHandle",                        // tp_name
    sizeof(CalloutHandleObject),                // tp_basicsize
    0,                                          // tp_itemsize
    (destructor) CalloutHandle_dealloc,         // tp_dealloc
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
    "Kea server CalloutHandle",                 // tp_doc
    0,                                          // tp_traverse
    0,                                          // tp_clear
    0,                                          // tp_richcompare
    0,                                          // tp_weaklistoffset
    0,                                          // tp_iter
    0,                                          // tp_iternext
    CalloutHandle_methods,                      // tp_methods
    0,                                          // tp_members
    0,                                          // tp_getset
    0,                                          // tp_base
    0,                                          // tp_dict
    0,                                          // tp_descr_get
    0,                                          // tp_descr_set
    0,                                          // tp_dictoffset
    (initproc) CalloutHandle_init,              // tp_init
    PyType_GenericAlloc,                        // tp_alloc
    CalloutHandle_new                           // tp_new
};

int
CalloutHandle_Check(PyObject *object) {
    return (Py_TYPE(object) == &CalloutHandleType);
}

PyObject *
CalloutHandle_from_handle(CalloutHandle *handle) {
    CalloutHandleObject *obj = PyObject_New(CalloutHandleObject, &CalloutHandleType);
    if (obj) {
        obj->handle = handle;
        obj->is_owner = false;
    }
    return (PyObject *)obj;
}

int
CalloutHandle_define() {
    if (PyType_Ready(&CalloutHandleType) < 0) {
        return (1);
    }
    Py_INCREF(&CalloutHandleType);
    if (PyModule_AddObject(kea_module, "CalloutHandle", (PyObject *) &CalloutHandleType) < 0) {
        Py_DECREF(&CalloutHandleType);
        return (1);
    }

    return (0);
}

}
