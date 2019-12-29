#include "keamodule.h"

using namespace std;
using namespace isc::hooks;
using namespace isc::dhcp;

extern "C" {

static PyMethodDef Lease4_methods[] = {
    {0}  // Sentinel
};

static PyObject *
Lease4_use_count(Lease4Object *self, void *closure) {
    return (PyLong_FromLong(self->ptr.use_count()));
}

static PyObject *
Lease4_get_addr(Lease4Object *self, void *closure) {
    try {
        string addr = self->ptr->addr_.toText();
        return (PyUnicode_FromString(addr.c_str()));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static int
Lease4_set_addr(Lease4Object *self, PyObject *value, void *closure) {
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the addr attribute");
        return (-1);
    }
    if (!PyUnicode_Check(value)) {
        PyErr_SetString(PyExc_TypeError, "The addr attribute value must be a string");
        return (-1);
    }
    try {
        const char *addr = PyUnicode_AsUTF8(value);
        self->ptr->addr_ = isc::asiolink::IOAddress(string(addr));
        return (0);
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (-1);
    }
}

static PyGetSetDef Lease4_getsetters[] = {
    {(char *)"use_count", (getter) Lease4_use_count, (setter) 0, (char *)"shared_ptr use count", 0},
    {(char *)"addr", (getter) Lease4_get_addr, (setter) Lease4_set_addr, (char *)"address in string form", 0},
    {0}  // Sentinel
};

static void
Lease4_dealloc(Lease4Object *self) {
    self->ptr.reset();
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static int
Lease4_init(Lease4Object *self, PyObject *args, PyObject *kwds) {
    static const char *kwlist[] = {0};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "", (char**)kwlist)) {
        return (-1);
    }
    self->ptr.reset(new Lease4());

    return (0);
}

static PyObject *
Lease4_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    Lease4Object *self;
    self = (Lease4Object *) type->tp_alloc(type, 0);
    if (self) {
        self->ptr.reset();
    }
    return ((PyObject *) self);
}

static PyTypeObject Lease4Type = {
    PyObject_HEAD_INIT(0)
    "kea.Lease4",                               // tp_name
    sizeof(Lease4Object),                       // tp_basicsize
    0,                                          // tp_itemsize
    (destructor) Lease4_dealloc,                // tp_dealloc
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
    "Kea server Lease4",                        // tp_doc
    0,                                          // tp_traverse
    0,                                          // tp_clear
    0,                                          // tp_richcompare
    0,                                          // tp_weaklistoffset
    0,                                          // tp_iter
    0,                                          // tp_iternext
    Lease4_methods,                             // tp_methods
    0,                                          // tp_members
    Lease4_getsetters,                          // tp_getset
    0,                                          // tp_base
    0,                                          // tp_dict
    0,                                          // tp_descr_get
    0,                                          // tp_descr_set
    0,                                          // tp_dictoffset
    (initproc) Lease4_init,                     // tp_init
    PyType_GenericAlloc,                        // tp_alloc
    Lease4_new                                  // tp_new
};

int
Lease4_Check(PyObject *object) {
    return (Py_TYPE(object) == &Lease4Type);
}

PyObject *
Lease4_from_handle(Lease4Ptr &ptr) {
    Lease4Object *self = PyObject_New(Lease4Object, &Lease4Type);
    if (self) {
        memset(&self->ptr, 0 , sizeof(self->ptr));
        self->ptr = ptr;
    }
    return (PyObject *)self;
}

int
Lease4_define() {
    if (PyType_Ready(&Lease4Type) < 0) {
        return (1);
    }
    Py_INCREF(&Lease4Type);
    if (PyModule_AddObject(kea_module, "Lease4", (PyObject *) &Lease4Type) < 0) {
        Py_DECREF(&Lease4Type);
        return (1);
    }

    return (0);
}

}
