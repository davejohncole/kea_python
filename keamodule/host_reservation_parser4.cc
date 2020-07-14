#include "keamodule.h"

using namespace std;
using namespace isc::dhcp;
using namespace isc::data;

extern "C" {

static PyObject *
HostReservationParser4_parse(HostReservationParser4Object *self, PyObject *args) {
    unsigned long subnet_id;
    PyObject *config;

    if (!PyArg_ParseTuple(args, "kO", &subnet_id, &config)) {
        return (0);
    }
    try {
        ElementPtr data = object_to_element(config);
        HostPtr host = self->parser->parse(subnet_id, data);
        return Host_from_ptr(host);
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}


static PyMethodDef HostReservationParser4_methods[] = {
    {"parse", (PyCFunction) HostReservationParser4_parse, METH_VARARGS,
     "Parses a single entry for host reservation."},
    {0}  // Sentinel
};

static void
HostReservationParser4_dealloc(HostReservationParser4Object *self) {
    delete self->parser;
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static int
HostReservationParser4_init(HostReservationParser4Object *self, PyObject *args, PyObject *kwds) {
    if (kwds != 0) {
        PyErr_SetString(PyExc_TypeError, "keyword arguments are not supported");
        return (-1);
    }
    if (!PyArg_ParseTuple(args, "")) {
        return (-1);
    }

    return (0);
}

static PyObject *
HostReservationParser4_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    HostReservationParser4Object *self;
    self = (HostReservationParser4Object *) type->tp_alloc(type, 0);
    self->parser = new HostReservationParser4();
    return ((PyObject *) self);
}

PyTypeObject HostReservationParser4Type = {
    PyObject_HEAD_INIT(0)
    "kea.HostReservationParser4",               // tp_name
    sizeof(HostReservationParser4Object),       // tp_basicsize
    0,                                          // tp_itemsize
    (destructor) HostReservationParser4_dealloc,// tp_dealloc
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
    "Kea server HostReservationParser4",        // tp_doc
    0,                                          // tp_traverse
    0,                                          // tp_clear
    0,                                          // tp_richcompare
    0,                                          // tp_weaklistoffset
    0,                                          // tp_iter
    0,                                          // tp_iternext
    HostReservationParser4_methods,             // tp_methods
    0,                                          // tp_members
    0,                                          // tp_getset
    0,                                          // tp_base
    0,                                          // tp_dict
    0,                                          // tp_descr_get
    0,                                          // tp_descr_set
    0,                                          // tp_dictoffset
    (initproc) HostReservationParser4_init,     // tp_init
    PyType_GenericAlloc,                        // tp_alloc
    HostReservationParser4_new                  // tp_new
};

int
HostReservationParser4_define() {
    if (PyType_Ready(&HostReservationParser4Type) < 0) {
        return (1);
    }
    Py_INCREF(&HostReservationParser4Type);
    if (PyModule_AddObject(kea_module, "HostReservationParser4", (PyObject *) &HostReservationParser4Type) < 0) {
        Py_DECREF(&HostReservationParser4Type);
        return (1);
    }

    return (0);
}

}
