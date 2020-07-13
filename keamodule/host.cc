#include "keamodule.h"

using namespace std;
using namespace isc::dhcp;
using namespace isc::asiolink;
using namespace isc::data;

extern "C" {

static PyObject *
Host_toElement(HostObject *self, PyObject *args) {
    try {
        ElementPtr ptr = (self->is_const ? self->const_ptr : self->ptr)->toElement4();
        return (element_to_object(ptr));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyMethodDef Host_methods[] = {
    {"toElement", (PyCFunction) Host_toElement, METH_NOARGS,
     "Element representation of the host."},
    {0}  // Sentinel
};

static PyObject *
Host_use_count(HostObject *self, void *closure) {
    return (PyLong_FromLong(self->is_const ? self->const_ptr.use_count() : self->ptr.use_count()));
}

static PyGetSetDef Host_getsetters[] = {
    {(char *)"use_count", (getter) Host_use_count, (setter) 0, (char *)"shared_ptr use count", 0},
    {0}  // Sentinel
};

static void
Host_dealloc(HostObject *self) {
    self->ptr.reset();
    self->const_ptr.reset();
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static int
Host_init(HostObject *self, PyObject *args, PyObject *kwds) {
    static const char *kwlist[] = {"identifier", "identifier_type", "subnet_id", "ipv4_reservation", NULL};
    const char *identifier;
    const char *identifier_type;
    unsigned long subnet_id;
    const char *ipv4_reservation;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ssks", (char **)kwlist,
         &identifier, &identifier_type, &subnet_id, &ipv4_reservation)) {
        return (-1);
    }

    try {
        self->ptr.reset(new Host(identifier, identifier_type, subnet_id, 0, IOAddress(string(ipv4_reservation))));
        self->const_ptr.reset();
        self->is_const = false;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (-1);
    }

    return (0);
}

static PyObject *
Host_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    HostObject *self;
    self = (HostObject *) type->tp_alloc(type, 0);
    if (self) {
        self->ptr.reset();
        self->const_ptr.reset();
    }
    return ((PyObject *) self);
}

PyTypeObject HostType = {
    PyObject_HEAD_INIT(0)
    "kea.Host",                                 // tp_name
    sizeof(HostObject),                         // tp_basicsize
    0,                                          // tp_itemsize
    (destructor) Host_dealloc,                  // tp_dealloc
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
    "Kea server Host",                          // tp_doc
    0,                                          // tp_traverse
    0,                                          // tp_clear
    0,                                          // tp_richcompare
    0,                                          // tp_weaklistoffset
    0,                                          // tp_iter
    0,                                          // tp_iternext
    Host_methods,                               // tp_methods
    0,                                          // tp_members
    Host_getsetters,                            // tp_getset
    0,                                          // tp_base
    0,                                          // tp_dict
    0,                                          // tp_descr_get
    0,                                          // tp_descr_set
    0,                                          // tp_dictoffset
    (initproc) Host_init,                       // tp_init
    PyType_GenericAlloc,                        // tp_alloc
    Host_new                                    // tp_new
};

PyObject *
Host_from_ptr(HostPtr host) {
    HostObject *self = PyObject_New(HostObject, &HostType);
    if (self) {
        memset(&self->ptr, 0 , sizeof(self->ptr));
        memset(&self->const_ptr, 0 , sizeof(self->const_ptr));
        self->is_const = false;
        self->ptr = host;
    }
    return (PyObject *)self;
}

PyObject *
Host_from_constptr(ConstHostPtr host) {
    HostObject *self = PyObject_New(HostObject, &HostType);
    if (self) {
        memset(&self->ptr, 0 , sizeof(self->ptr));
        memset(&self->const_ptr, 0 , sizeof(self->const_ptr));
        self->is_const = true;
        self->const_ptr = host;
    }
    return (PyObject *)self;
}

int
Host_define() {
    if (PyType_Ready(&HostType) < 0) {
        return (1);
    }
    Py_INCREF(&HostType);
    if (PyModule_AddObject(kea_module, "Host", (PyObject *) &HostType) < 0) {
        Py_DECREF(&HostType);
        return (1);
    }

    return (0);
}

}
