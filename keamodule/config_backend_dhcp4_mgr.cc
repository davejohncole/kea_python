#include "keamodule.h"

using namespace std;
using namespace isc::dhcp;
using namespace isc::asiolink;
using namespace isc::util;

extern "C" {

static PyObject *
ConfigBackendDHCPv4Mgr_addBackend(ConfigBackendDHCPv4MgrObject *self, PyObject *args) {
    const char *dbaccess;

    if (!PyArg_ParseTuple(args, "s", &dbaccess)) {
        return (0);
    }

    try {
        self->mgr->addBackend(dbaccess);
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
ConfigBackendDHCPv4Mgr_getPool(ConfigBackendDHCPv4MgrObject *self, PyObject *args) {
    try {
        boost::shared_ptr<isc::dhcp::ConfigBackendPoolDHCPv4> pool = self->mgr->getPool();
        return (ConfigBackendPoolDHCPv4_from_ptr(pool));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

PyObject *
ConfigBackendDHCPv4Mgr_from_ptr(ConfigBackendDHCPv4Mgr *mgr) {
    ConfigBackendDHCPv4MgrObject *self = PyObject_New(ConfigBackendDHCPv4MgrObject, &ConfigBackendDHCPv4MgrType);
    if (self) {
        self->mgr = mgr;
    }
    return ((PyObject *)self);
}

static PyObject *
ConfigBackendDHCPv4Mgr_instance(ConfigBackendDHCPv4MgrObject *self, PyObject *args) {
    try {
        ConfigBackendDHCPv4Mgr& mgr = ConfigBackendDHCPv4Mgr::instance();
        return (ConfigBackendDHCPv4Mgr_from_ptr(&mgr));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}


static PyMethodDef ConfigBackendDHCPv4Mgr_methods[] = {
    {"instance", (PyCFunction) ConfigBackendDHCPv4Mgr_instance, METH_NOARGS|METH_STATIC,
     "Returns a sole instance of the ConfigBackendDHCPv4Mgr."},
    {"addBackend", (PyCFunction) ConfigBackendDHCPv4Mgr_addBackend, METH_VARARGS,
     "Create an instance of a configuration backend."},
    {"getPool", (PyCFunction) ConfigBackendDHCPv4Mgr_getPool, METH_NOARGS,
     "Attempts to delete a host by (subnet4-id, identifier, identifier-type)."},
    {0}  // Sentinel
};

static int
ConfigBackendDHCPv4Mgr_init(ConfigBackendDHCPv4MgrObject *self, PyObject *args, PyObject *kwds) {
    PyErr_SetString(PyExc_RuntimeError, "cannot directly construct");
    return (-1);
}

PyTypeObject ConfigBackendDHCPv4MgrType = {
    PyObject_HEAD_INIT(0)
    "kea.ConfigBackendDHCPv4Mgr",               // tp_name
    sizeof(ConfigBackendDHCPv4MgrObject),       // tp_basicsize
    0,                                          // tp_itemsize
    0,                                          // tp_dealloc
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
    "Kea server ConfigBackendDHCPv4Mgr",        // tp_doc
    0,                                          // tp_traverse
    0,                                          // tp_clear
    0,                                          // tp_richcompare
    0,                                          // tp_weaklistoffset
    0,                                          // tp_iter
    0,                                          // tp_iternext
    ConfigBackendDHCPv4Mgr_methods,             // tp_methods
    0,                                          // tp_members
    0,                                          // tp_getset
    0,                                          // tp_base
    0,                                          // tp_dict
    0,                                          // tp_descr_get
    0,                                          // tp_descr_set
    0,                                          // tp_dictoffset
    (initproc) ConfigBackendDHCPv4Mgr_init,     // tp_init
    PyType_GenericAlloc,                        // tp_alloc
    PyType_GenericNew                           // tp_new
};

int
ConfigBackendDHCPv4Mgr_define() {
    if (PyType_Ready(&ConfigBackendDHCPv4MgrType) < 0) {
        return (1);
    }
    Py_INCREF(&ConfigBackendDHCPv4MgrType);
    if (PyModule_AddObject(kea_module, "ConfigBackendDHCPv4Mgr", (PyObject *) &ConfigBackendDHCPv4MgrType) < 0) {
        Py_DECREF(&ConfigBackendDHCPv4MgrType);
        return (1);
    }

    return (0);
}

}