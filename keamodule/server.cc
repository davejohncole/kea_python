#include "keamodule.h"

using namespace std;
using namespace isc::db;
using namespace isc::data;

extern "C" {

static PyObject *
Server_getServerTagAsText(ServerObject *self, PyObject *args) {
    try {
        string tag = self->ptr->getServerTagAsText();
        // REFCOUNT: PyUnicode_FromString - returns new reference
        return (PyUnicode_FromString(tag.c_str()));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Server_getDescription(ServerObject *self, PyObject *args) {
    try {
        string description = self->ptr->getDescription();
        // REFCOUNT: PyUnicode_FromString - returns new reference
        return (PyUnicode_FromString(description.c_str()));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Server_toElement(ServerObject *self, PyObject *args) {
    try {
        ElementPtr ptr = self->ptr->toElement();
        // REFCOUNT: element_to_object - returns new reference
        return (element_to_object(ptr));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyMethodDef Server_methods[] = {
    {"getServerTagAsText", (PyCFunction) Server_getServerTagAsText, METH_NOARGS,
     "Returns server tag as text."},
    {"getDescription", (PyCFunction) Server_getDescription, METH_NOARGS,
     "Returns the description of the server."},
    {"toElement", (PyCFunction) Server_toElement, METH_NOARGS,
     "Unparses server object."},
    {0}  // Sentinel
};

static PyObject *
Server_use_count(ServerObject *self, void *closure) {
    // REFCOUNT: PyLong_FromLong - returns new reference
    return (PyLong_FromLong(self->ptr.use_count()));
}

static PyGetSetDef Server_getsetters[] = {
    {(char *)"use_count", (getter) Server_use_count, (setter) 0, (char *)"shared_ptr use count", 0},
    {0}  // Sentinel
};

// tp_dealloc - called when refcount is zero
static void
Server_dealloc(ServerObject *self) {
    self->ptr.~ServerPtr();
    Py_TYPE(self)->tp_free((PyObject *) self);
}

// tp_init - called after tp_new has returned an instance
static int
Server_init(ServerObject *self, PyObject *args, PyObject *kwds) {
    const char *tag;
    const char *description;

    new(&self->ptr) ServerPtr;

    if (kwds != 0) {
        PyErr_SetString(PyExc_TypeError, "keyword arguments are not supported");
        return (-1);
    }

    if (!PyArg_ParseTuple(args, "ss", &tag, &description)) {
        return (-1);
    }
    self->ptr.reset(new Server(ServerTag(tag), description));

    return (0);
}

// tp_new - allocate space and initialisation that can be repeated
static PyObject *
Server_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    ServerObject *self;
    self = (ServerObject *) type->tp_alloc(type, 0);
    if (self) {
        new(&self->ptr) ServerPtr;
    }
    return ((PyObject *) self);
}

PyTypeObject ServerType = {
    PyObject_HEAD_INIT(0)
    "kea.Server",                               // tp_name
    sizeof(ServerObject),                       // tp_basicsize
    0,                                          // tp_itemsize
    (destructor) Server_dealloc,                // tp_dealloc
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
    "Kea server Server",                        // tp_doc
    0,                                          // tp_traverse
    0,                                          // tp_clear
    0,                                          // tp_richcompare
    0,                                          // tp_weaklistoffset
    0,                                          // tp_iter
    0,                                          // tp_iternext
    Server_methods,                             // tp_methods
    0,                                          // tp_members
    Server_getsetters,                          // tp_getset
    0,                                          // tp_base
    0,                                          // tp_dict
    0,                                          // tp_descr_get
    0,                                          // tp_descr_set
    0,                                          // tp_dictoffset
    (initproc) Server_init,                     // tp_init
    PyType_GenericAlloc,                        // tp_alloc
    Server_new                                  // tp_new
};

PyObject *
Server_from_ptr(ServerPtr &ptr) {
    // REFCOUNT: PyObject_New - returns new reference
    ServerObject *self = PyObject_New(ServerObject, &ServerType);
    if (self) {
        new(&self->ptr) ServerPtr;
        self->ptr = ptr;
    }
    return (PyObject *)self;
}

int
Server_define() {
    // PyType_Ready - finish type initialisation
    if (PyType_Ready(&ServerType) < 0) {
        return (1);
    }
    Py_INCREF(&ServerType);
    // REFCOUNT: PyModule_AddObject steals reference on success
    if (PyModule_AddObject(kea_module, "Server", (PyObject *) &ServerType) < 0) {
        Py_DECREF(&ServerType);
        return (1);
    }

    return (0);
}

}
