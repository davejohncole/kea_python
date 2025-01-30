#include <ffi.h>
#include "keamodule.h"

using namespace std;
using namespace isc::hooks;

extern "C" {

static PyObject *
LibraryHandle_registerCommandCallout(LibraryHandleObject *self, PyObject *args) {
    PyObject *name;
    PyObject *callout;

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
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

    // REFCOUNT: CalloutClosure_from_object - returns new reference
    CalloutClosureObject *obj = (CalloutClosureObject *) CalloutClosure_from_object(name, callout);
    if (!obj) {
        return (0);
    }
    // REFCOUNT: transfer ownership of obj to dict in callouts.cc
    if (Callouts_add_closure(obj)) {
        Py_DECREF(obj);
        return (0);
    }
    Py_DECREF(obj);
    try {
        // REFCOUNT: PyUnicode_AsUTF8 - returns UTF-8 encoding of str - buffer cached in str
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

// tp_dealloc - called when refcount is zero
static void
LibraryHandle_dealloc(LibraryHandleObject *self) {
    if (self->is_owner) {
        delete self->handle;
    }
    Py_TYPE(self)->tp_free((PyObject *) self);
}

// tp_init - called after tp_new has returned an instance
static int
LibraryHandle_init(LibraryHandleObject *self, PyObject *args, PyObject *kwds) {
    CalloutManagerObject *manager = 0;

    if (kwds != 0) {
        PyErr_SetString(PyExc_TypeError, "keyword arguments are not supported");
        return (-1);
    }
    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
    if (!PyArg_ParseTuple(args, "O!", &CalloutManagerType, &manager)) {
        return (-1);
    }

    try {
#if HAVE_LIBRARYHANDLE_MANAGER_PTR
        self->handle = new LibraryHandle(manager->manager.get());
#else
        self->handle = new LibraryHandle(*manager->manager);
#endif
        self->is_owner = true;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (-1);
    }

    return (0);
}

// tp_new - allocate space and initialisation that can be repeated
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
    .ob_base = PyObject_HEAD_INIT(0)
    .tp_name = "kea.LibraryHandle",
    .tp_basicsize = sizeof(LibraryHandleObject),
    .tp_dealloc = (destructor) LibraryHandle_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Kea server LibraryHandle"),
    .tp_methods = LibraryHandle_methods,
    .tp_init = (initproc) LibraryHandle_init,
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = LibraryHandle_new,
};

PyObject *
LibraryHandle_from_handle(LibraryHandle *handle) {
    // REFCOUNT: PyObject_New - returns new reference
    LibraryHandleObject *obj = PyObject_New(LibraryHandleObject, &LibraryHandleType);
    if (obj) {
        obj->handle = handle;
        obj->is_owner = false;
    }
    return (PyObject *)obj;
}

int
LibraryHandle_define() {
    // PyType_Ready - finish type initialisation
    if (PyType_Ready(&LibraryHandleType) < 0) {
        return (1);
    }
    Py_INCREF(&LibraryHandleType);
    // REFCOUNT: PyModule_AddObject steals reference on success
    if (PyModule_AddObject(kea_module, "LibraryHandle", (PyObject *) &LibraryHandleType) < 0) {
        Py_DECREF(&LibraryHandleType);
        return (1);
    }

    return (0);
}

}
