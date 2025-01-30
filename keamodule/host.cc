#include "keamodule.h"

using namespace std;
using namespace isc::dhcp;
using namespace isc::asiolink;
using namespace isc::data;

extern "C" {

static PyObject *
Host_getHostId(HostObject *self, PyObject *args) {
    try {
        HostID host_id = (self->is_const ? self->const_ptr : self->ptr)->getHostId();
        // REFCOUNT: PyLong_FromUnsignedLongLong - returns new reference
        return (PyLong_FromUnsignedLongLong(host_id));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Host_toElement(HostObject *self, PyObject *args) {
    try {
        ElementPtr ptr = (self->is_const ? self->const_ptr : self->ptr)->toElement4();
        // element_to_object - returns new reference
        return (element_to_object(ptr));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyMethodDef Host_methods[] = {
    {"getHostId", (PyCFunction) Host_getHostId, METH_NOARGS,
     "Returns Host ID (primary key in MySQL, PostgreSQL and Cassandra backends)."},
    {"toElement", (PyCFunction) Host_toElement, METH_NOARGS,
     "Element representation of the host."},
    {0}  // Sentinel
};

static PyObject *
Host_use_count(HostObject *self, void *closure) {
    // REFCOUNT: PyLong_FromLong - returns new reference
    return (PyLong_FromLong(self->is_const ? self->const_ptr.use_count() : self->ptr.use_count()));
}

static PyGetSetDef Host_getsetters[] = {
    {(char *)"use_count", (getter) Host_use_count, (setter) 0, (char *)"shared_ptr use count", 0},
    {0}  // Sentinel
};

// tp_dealloc - called when refcount is zero
static void
Host_dealloc(HostObject *self) {
    self->ptr.~HostPtr();
    self->const_ptr.~ConstHostPtr();
    Py_TYPE(self)->tp_free((PyObject *) self);
}

// tp_init - called after tp_new has returned an instance
static int
Host_init(HostObject *self, PyObject *args, PyObject *kwds) {
    static const char *kwlist[] = {"identifier", "identifier_type", "subnet_id", "ipv4_reservation", NULL};
    const char *identifier;
    const char *identifier_type;
    unsigned long subnet_id;
    const char *ipv4_reservation;

    new(&self->ptr) HostPtr;
    new(&self->const_ptr) ConstHostPtr;

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

// tp_new - allocate space and initialisation that can be repeated
static PyObject *
Host_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    HostObject *self;
    self = (HostObject *) type->tp_alloc(type, 0);
    if (self) {
        new(&self->ptr) HostPtr;
        new(&self->const_ptr) ConstHostPtr;
    }
    return ((PyObject *) self);
}

PyTypeObject HostType = {
    .ob_base = PyObject_HEAD_INIT(0)
    .tp_name = "kea.Host",
    .tp_basicsize = sizeof(HostObject),
    .tp_dealloc = (destructor) Host_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Kea server Host"),
    .tp_methods = Host_methods,
    .tp_getset = Host_getsetters,
    .tp_init = (initproc) Host_init,
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = Host_new,
};

PyObject *
Host_from_ptr(HostPtr host) {
    // REFCOUNT: PyObject_New - returns new reference
    HostObject *self = PyObject_New(HostObject, &HostType);
    if (self) {
        new(&self->ptr) HostPtr;
        new(&self->const_ptr) ConstHostPtr;
        self->is_const = false;
        self->ptr = host;
    }
    return (PyObject *)self;
}

PyObject *
Host_from_constptr(ConstHostPtr host) {
    // REFCOUNT: PyObject_New - returns new reference
    HostObject *self = PyObject_New(HostObject, &HostType);
    if (self) {
        new(&self->ptr) HostPtr;
        new(&self->const_ptr) ConstHostPtr;
        self->is_const = true;
        self->const_ptr = host;
    }
    return (PyObject *)self;
}

int
Host_define() {
    // PyType_Ready - finish type initialisation
    if (PyType_Ready(&HostType) < 0) {
        return (1);
    }
    Py_INCREF(&HostType);
    // REFCOUNT: PyModule_AddObject steals reference on success
    if (PyModule_AddObject(kea_module, "Host", (PyObject *) &HostType) < 0) {
        Py_DECREF(&HostType);
        return (1);
    }

    return (0);
}

}
