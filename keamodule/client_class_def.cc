#include "keamodule.h"

using namespace std;
using namespace isc::dhcp;

extern "C" {

static bool
ClientClassDef_is_initialized(ClientClassDefObject *self) {
    if (!self->ptr) {
        // REFCOUNT: PyErr_SetString - does not affect reference counts
        PyErr_SetString(PyExc_RuntimeError, "ClientClassDef object is not initialized");
        return false;
    }
    return true;
}

static PyObject *
ClientClassDef_getName(ClientClassDefObject *self, PyObject *Py_UNUSED(ignored)) {
    try {
        if (!ClientClassDef_is_initialized(self)) {
            return (0);
        }
        const string &name = self->ptr->getName();
        // REFCOUNT: PyUnicode_FromString - returns new reference
        return PyUnicode_FromString(name.c_str());
    }
    catch (const exception &e) {
        // REFCOUNT: PyErr_SetString - does not affect reference counts
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
ClientClassDef_toElement(ClientClassDefObject *self, PyObject *Py_UNUSED(ignored)) {
    try {
        if (!ClientClassDef_is_initialized(self)) {
            return (0);
        }
        isc::data::ElementPtr element = self->ptr->toElement();
        // REFCOUNT: element_to_object - returns new reference
        return element_to_object(element);
    }
    catch (const exception &e) {
        // REFCOUNT: PyErr_SetString - does not affect reference counts
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyMethodDef ClientClassDef_methods[] = {
    {"getName", (PyCFunction) ClientClassDef_getName, METH_NOARGS,
     "Returns the name of the client class."},
    {"toElement", (PyCFunction) ClientClassDef_toElement, METH_NOARGS,
     "Returns the client class definition as a Python dictionary."},
    {0}  // Sentinel
};

// tp_dealloc - called when refcount is zero
static void
ClientClassDef_dealloc(ClientClassDefObject *self) {
    self->ptr.~ClientClassDefPtr();
    // REFCOUNT: tp_free - frees object
    Py_TYPE(self)->tp_free((PyObject *) self);
}

// tp_init - called after tp_new has returned an instance
static int
ClientClassDef_init(ClientClassDefObject *self, PyObject *args, PyObject *kwds) {
    if (kwds != 0) {
        // REFCOUNT: PyErr_SetString - does not affect reference counts
        PyErr_SetString(PyExc_TypeError, "keyword arguments are not supported");
        return (-1);
    }
    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
    if (!PyArg_ParseTuple(args, "")) {
        return (-1);
    }

    return (0);
}

// tp_new - allocate space and initialisation that can be repeated
static PyObject *
ClientClassDef_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    ClientClassDefObject *self;
    // REFCOUNT: tp_alloc - returns new reference
    self = (ClientClassDefObject *) type->tp_alloc(type, 0);
    if (self) {
        new (&self->ptr) ClientClassDefPtr();
    }
    return ((PyObject *) self);
}

PyTypeObject ClientClassDefType = {
    .ob_base = PyObject_HEAD_INIT(0)
    .tp_name = "kea.ClientClassDef",
    .tp_basicsize = sizeof(ClientClassDefObject),
    .tp_dealloc = (destructor) ClientClassDef_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Kea server ClientClassDef"),
    .tp_methods = ClientClassDef_methods,
    .tp_init = (initproc) ClientClassDef_init,
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = ClientClassDef_new,
};

PyObject *
ClientClassDef_from_ptr(ClientClassDefPtr &ptr) {
    // REFCOUNT: tp_alloc - returns new reference
    ClientClassDefObject *obj = (ClientClassDefObject *) ClientClassDefType.tp_alloc(&ClientClassDefType, 0);
    new (&obj->ptr) ClientClassDefPtr(ptr);
    return (PyObject *) obj;
}

int
ClientClassDef_define() {
    // PyType_Ready - finish type initialisation
    if (PyType_Ready(&ClientClassDefType) < 0) {
        return (1);
    }
    Py_INCREF(&ClientClassDefType);
    // REFCOUNT: PyModule_AddObject steals reference on success
    if (PyModule_AddObject(kea_module, "ClientClassDef", (PyObject *) &ClientClassDefType) < 0) {
        Py_DECREF(&ClientClassDefType);
        return (1);
    }

    return (0);
}

}