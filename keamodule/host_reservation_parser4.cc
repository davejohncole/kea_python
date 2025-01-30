#include "keamodule.h"

using namespace std;
using namespace isc::dhcp;
using namespace isc::data;

extern "C" {

static PyObject *
HostReservationParser4_parse(HostReservationParser4Object *self, PyObject *args) {
    unsigned long subnet_id;
    PyObject *config;

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
    if (!PyArg_ParseTuple(args, "kO", &subnet_id, &config)) {
        return (0);
    }
    try {
        ElementPtr data = object_to_element(config);
        HostPtr host = self->parser->parse(subnet_id, data);
        // REFCOUNT: Host_from_ptr - returns new reference
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

// tp_dealloc - called when refcount is zero
static void
HostReservationParser4_dealloc(HostReservationParser4Object *self) {
    delete self->parser;
    Py_TYPE(self)->tp_free((PyObject *) self);
}

// tp_init - called after tp_new has returned an instance
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

// tp_new - allocate space and initialisation that can be repeated
static PyObject *
HostReservationParser4_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    HostReservationParser4Object *self;
    self = (HostReservationParser4Object *) type->tp_alloc(type, 0);
    self->parser = new HostReservationParser4();
    return ((PyObject *) self);
}

PyTypeObject HostReservationParser4Type = {
    .ob_base = PyObject_HEAD_INIT(0)
    .tp_name = "kea.HostReservationParser4",
    .tp_basicsize = sizeof(HostReservationParser4Object),
    .tp_dealloc = (destructor) HostReservationParser4_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Kea server HostReservationParser4"),
    .tp_methods = HostReservationParser4_methods,
    .tp_init = (initproc) HostReservationParser4_init,
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = HostReservationParser4_new,
};

int
HostReservationParser4_define() {
    // PyType_Ready - finish type initialisation
    if (PyType_Ready(&HostReservationParser4Type) < 0) {
        return (1);
    }
    Py_INCREF(&HostReservationParser4Type);
    // REFCOUNT: PyModule_AddObject steals reference on success
    if (PyModule_AddObject(kea_module, "HostReservationParser4", (PyObject *) &HostReservationParser4Type) < 0) {
        Py_DECREF(&HostReservationParser4Type);
        return (1);
    }

    return (0);
}

}
