#include "keamodule.h"

using namespace std;
using namespace isc::dhcp;
using namespace isc::data;

extern "C" {

static PyObject *
Subnet4ConfigParser_parse(Subnet4ConfigParserObject *self, PyObject *args) {
    PyObject *config;

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
    if (!PyArg_ParseTuple(args, "O", &config)) {
        return (0);
    }
    try {
        ElementPtr data = object_to_element(config);
        Subnet4Ptr subnet = self->parser->parse(data);
        // REFCOUNT: Subnet4_from_ptr - returns new reference
        return Subnet4_from_ptr(subnet);
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}


static PyMethodDef Subnet4ConfigParser_methods[] = {
    {"parse", (PyCFunction) Subnet4ConfigParser_parse, METH_VARARGS,
     "Parses a single IPv4 subnet configuration and adds to the Configuration Manager."},
    {0}  // Sentinel
};

// tp_dealloc - called when refcount is zero
static void
Subnet4ConfigParser_dealloc(Subnet4ConfigParserObject *self) {
    delete self->parser;
    Py_TYPE(self)->tp_free((PyObject *) self);
}

// tp_init - called after tp_new has returned an instance
static int
Subnet4ConfigParser_init(Subnet4ConfigParserObject *self, PyObject *args, PyObject *kwds) {
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
Subnet4ConfigParser_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    Subnet4ConfigParserObject *self;
    self = (Subnet4ConfigParserObject *) type->tp_alloc(type, 0);
    self->parser = new Subnet4ConfigParser();
    return ((PyObject *) self);
}

PyTypeObject Subnet4ConfigParserType = {
    .ob_base = PyObject_HEAD_INIT(0)
    .tp_name = "kea.Subnet4ConfigParser",
    .tp_basicsize = sizeof(Subnet4ConfigParserObject),
    .tp_dealloc = (destructor) Subnet4ConfigParser_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Kea server Subnet4ConfigParser"),
    .tp_methods = Subnet4ConfigParser_methods,
    .tp_init = (initproc) Subnet4ConfigParser_init,
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = Subnet4ConfigParser_new,
};

int
Subnet4ConfigParser_define() {
    // PyType_Ready - finish type initialisation
    if (PyType_Ready(&Subnet4ConfigParserType) < 0) {
        return (1);
    }
    Py_INCREF(&Subnet4ConfigParserType);
    // REFCOUNT: PyModule_AddObject steals reference on success
    if (PyModule_AddObject(kea_module, "Subnet4ConfigParser", (PyObject *) &Subnet4ConfigParserType) < 0) {
        Py_DECREF(&Subnet4ConfigParserType);
        return (1);
    }

    return (0);
}

}
