#include "keamodule.h"

using namespace std;
using namespace isc::dhcp;
using namespace isc::data;

extern "C" {

static PyObject *
Subnet4ConfigParser_parse(Subnet4ConfigParserObject *self, PyObject *args) {
    PyObject *config;

    if (!PyArg_ParseTuple(args, "O", &config)) {
        return (0);
    }
    try {
        ElementPtr data = object_to_element(config);
        Subnet4Ptr subnet = self->parser->parse(data);
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

static void
Subnet4ConfigParser_dealloc(Subnet4ConfigParserObject *self) {
    delete self->parser;
    Py_TYPE(self)->tp_free((PyObject *) self);
}

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

static PyObject *
Subnet4ConfigParser_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    Subnet4ConfigParserObject *self;
    self = (Subnet4ConfigParserObject *) type->tp_alloc(type, 0);
    self->parser = new Subnet4ConfigParser();
    return ((PyObject *) self);
}

PyTypeObject Subnet4ConfigParserType = {
    PyObject_HEAD_INIT(0)
    "kea.Subnet4ConfigParser",                  // tp_name
    sizeof(Subnet4ConfigParserObject),          // tp_basicsize
    0,                                          // tp_itemsize
    (destructor) Subnet4ConfigParser_dealloc,   // tp_dealloc
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
    "Kea server Subnet4ConfigParser",           // tp_doc
    0,                                          // tp_traverse
    0,                                          // tp_clear
    0,                                          // tp_richcompare
    0,                                          // tp_weaklistoffset
    0,                                          // tp_iter
    0,                                          // tp_iternext
    Subnet4ConfigParser_methods,                // tp_methods
    0,                                          // tp_members
    0,                                          // tp_getset
    0,                                          // tp_base
    0,                                          // tp_dict
    0,                                          // tp_descr_get
    0,                                          // tp_descr_set
    0,                                          // tp_dictoffset
    (initproc) Subnet4ConfigParser_init,        // tp_init
    PyType_GenericAlloc,                        // tp_alloc
    Subnet4ConfigParser_new                     // tp_new
};

int
Subnet4ConfigParser_define() {
    if (PyType_Ready(&Subnet4ConfigParserType) < 0) {
        return (1);
    }
    Py_INCREF(&Subnet4ConfigParserType);
    if (PyModule_AddObject(kea_module, "Subnet4ConfigParser", (PyObject *) &Subnet4ConfigParserType) < 0) {
        Py_DECREF(&Subnet4ConfigParserType);
        return (1);
    }

    return (0);
}

}
