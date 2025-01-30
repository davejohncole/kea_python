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
        // REFCOUNT: ConfigBackendPoolDHCPv4_from_ptr - returns new reference
        return (ConfigBackendPoolDHCPv4_from_ptr(pool));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

PyObject *
ConfigBackendDHCPv4Mgr_from_ptr(ConfigBackendDHCPv4Mgr *mgr) {
    // REFCOUNT: PyObject_New - returns new reference
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
        // REFCOUNT: ConfigBackendDHCPv4Mgr_from_ptr - returns new reference
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

// tp_init - called after tp_new has returned an instance
static int
ConfigBackendDHCPv4Mgr_init(ConfigBackendDHCPv4MgrObject *self, PyObject *args, PyObject *kwds) {
    PyErr_SetString(PyExc_TypeError, "cannot create 'kea.ConfigBackendDHCPv4Mgr' instances");
    return (-1);
}

PyTypeObject ConfigBackendDHCPv4MgrType = {
    .ob_base = PyObject_HEAD_INIT(0)
    .tp_name = "kea.ConfigBackendDHCPv4Mgr",
    .tp_basicsize = sizeof(ConfigBackendDHCPv4MgrObject),
    .tp_flags = Py_TPFLAGS_DEFAULT, 
    .tp_doc = PyDoc_STR("Kea server ConfigBackendDHCPv4Mgr"),
    .tp_methods = ConfigBackendDHCPv4Mgr_methods,
    .tp_init = (initproc) ConfigBackendDHCPv4Mgr_init,
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = PyType_GenericNew,
};

int
ConfigBackendDHCPv4Mgr_define() {
    // PyType_Ready - finish type initialisation
    if (PyType_Ready(&ConfigBackendDHCPv4MgrType) < 0) {
        return (1);
    }
    Py_INCREF(&ConfigBackendDHCPv4MgrType);
    // REFCOUNT: PyModule_AddObject steals reference on success
    if (PyModule_AddObject(kea_module, "ConfigBackendDHCPv4Mgr", (PyObject *) &ConfigBackendDHCPv4MgrType) < 0) {
        Py_DECREF(&ConfigBackendDHCPv4MgrType);
        return (1);
    }

    return (0);
}

}
