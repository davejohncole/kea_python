#include <ffi.h>
#include "keamodule.h"

using namespace std;
using namespace isc::hooks;

extern "C" {

static PyObject *
LibraryHandle_registerCommandCallout(LibraryHandleObject *self, PyObject *args) {
    PyObject *name;
    PyObject *callout;

    if (!PyArg_ParseTuple(args, "O!O", &PyUnicode_Type, &name, &callout)) {
        return (0);
    }
    if (!PyCallable_Check(callout)) {
        PyErr_SetString(PyExc_TypeError, "callout must be callable");
        return (0);
    }
    if (self->is_owner) {
        PyErr_SetString(PyExc_RuntimeError, "only supported in embedded mode");
        return (0);
    }

    CalloutClosureObject *obj = (CalloutClosureObject *) CalloutClosure_from_object(name, callout);
    if (!obj) {
        return (0);
    }
    // give ownership of obj to dict in callouts.cc
    if (Callouts_add_closure(obj)) {
        Py_DECREF(obj);
        return (0);
    }
    Py_DECREF(obj);
    try {
        self->handle->registerCommandCallout(PyUnicode_AsUTF8(name), (CalloutPtr)obj->bound_callout);
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }

    Py_RETURN_NONE;
}

static PyMethodDef LibraryHandle_methods[] = {
    {"registerCommandCallout", (PyCFunction) LibraryHandle_registerCommandCallout, METH_VARARGS,
     "Register control command handler."},
    {0}  // Sentinel
};

static void
LibraryHandle_dealloc(LibraryHandleObject *self) {
    if (self->is_owner) {
        delete self->handle;
    }
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static int
LibraryHandle_init(LibraryHandleObject *self, PyObject *args, PyObject *kwds) {
    CalloutManagerObject *manager = 0;

    if (kwds != 0) {
        PyErr_SetString(PyExc_TypeError, "keyword arguments are not supported");
        return (-1);
    }
    if (!PyArg_ParseTuple(args, "O!", &CalloutManagerType, &manager)) {
        return (-1);
    }

    try {
        self->handle = new LibraryHandle(manager->manager.get());
        self->is_owner = true;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (-1);
    }

    return (0);
}

static PyObject *
LibraryHandle_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    LibraryHandleObject *self;
    self = (LibraryHandleObject *) type->tp_alloc(type, 0);
    if (self) {
        self->handle = 0;
        self->is_owner = false;
    }
    return ((PyObject *) self);
}

PyTypeObject LibraryHandleType = {
    PyObject_HEAD_INIT(0)
    "kea.LibraryHandle",                        // tp_name
    sizeof(LibraryHandleObject),                // tp_basicsize
    0,                                          // tp_itemsize
    (destructor) LibraryHandle_dealloc,         // tp_dealloc
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
    "Kea server LibraryHandle",                 // tp_doc
    0,                                          // tp_traverse
    0,                                          // tp_clear
    0,                                          // tp_richcompare
    0,                                          // tp_weaklistoffset
    0,                                          // tp_iter
    0,                                          // tp_iternext
    LibraryHandle_methods,                      // tp_methods
    0,                                          // tp_members
    0,                                          // tp_getset
    0,                                          // tp_base
    0,                                          // tp_dict
    0,                                          // tp_descr_get
    0,                                          // tp_descr_set
    0,                                          // tp_dictoffset
    (initproc) LibraryHandle_init,              // tp_init
    PyType_GenericAlloc,                        // tp_alloc
    LibraryHandle_new                           // tp_new
};

PyObject *
LibraryHandle_from_handle(LibraryHandle *handle) {
    LibraryHandleObject *obj = PyObject_New(LibraryHandleObject, &LibraryHandleType);
    if (obj) {
        obj->handle = handle;
        obj->is_owner = false;
    }
    return (PyObject *)obj;
}

int
LibraryHandle_define() {
    if (PyType_Ready(&LibraryHandleType) < 0) {
        return (1);
    }
    Py_INCREF(&LibraryHandleType);
    if (PyModule_AddObject(kea_module, "LibraryHandle", (PyObject *) &LibraryHandleType) < 0) {
        Py_DECREF(&LibraryHandleType);
        return (1);
    }

    return (0);
}

}
