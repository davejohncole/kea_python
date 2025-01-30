#include "keamodule.h"

using namespace std;
using namespace isc::dhcp;

extern "C" {

static PyObject *
CfgMgr_getCurrentCfg(CfgMgrObject *self, PyObject *args) {
    try {
        SrvConfigPtr ptr = CfgMgr::instance().getCurrentCfg();
        // REFCOUNT: SrvConfig_from_ptr - returns new reference
        return (SrvConfig_from_ptr(ptr));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
CfgMgr_getStagingCfg(CfgMgrObject *self, PyObject *args) {
    try {
        SrvConfigPtr ptr = CfgMgr::instance().getStagingCfg();
        // REFCOUNT: SrvConfig_from_ptr - returns new reference
        return (SrvConfig_from_ptr(ptr));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

PyObject *
CfgMgr_from_ptr(CfgMgr *mgr) {
    // REFCOUNT: PyObject_New - returns new reference
    CfgMgrObject *self = PyObject_New(CfgMgrObject, &CfgMgrType);
    if (self) {
        self->mgr = mgr;
    }
    return ((PyObject *)self);
}

static PyObject *
CfgMgr_instance(CfgMgrObject *self, PyObject *args) {
    try {
        CfgMgr& mgr = CfgMgr::instance();
        // REFCOUNT: CfgMgr_from_ptr - returns new reference
        return (CfgMgr_from_ptr(&mgr));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyMethodDef CfgMgr_methods[] = {
    {"instance", (PyCFunction) CfgMgr_instance, METH_NOARGS|METH_STATIC,
     "Returns a single instance of Configuration Manager."
     " CfgMgr is a singleton and this method is the only way of accessing it."},
    {"getCurrentCfg", (PyCFunction) CfgMgr_getCurrentCfg, METH_NOARGS,
     "Returns the current configuration."},
    {"getStagingCfg", (PyCFunction) CfgMgr_getStagingCfg, METH_NOARGS,
     "Returns the staging configuration."},
    {0}  // Sentinel
};

// tp_dealloc - called when refcount is zero
static void
CfgMgr_dealloc(CfgMgrObject *self) {
    Py_TYPE(self)->tp_free((PyObject *) self);
}

// tp_init - called after tp_new has returned an instance
static int
CfgMgr_init(CfgMgrObject *self, PyObject *args, PyObject *kwds) {
    PyErr_SetString(PyExc_TypeError, "cannot create 'kea.CfgMgr' instances");
    return (-1);
}

PyTypeObject CfgMgrType = {
    .ob_base = PyObject_HEAD_INIT(0)
    .tp_name = "kea.CfgMgr",
    .tp_basicsize = sizeof(CfgMgrObject),
    .tp_dealloc = (destructor) CfgMgr_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Kea server CfgMgr"),
    .tp_methods = CfgMgr_methods,
    .tp_init = (initproc) CfgMgr_init,
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = PyType_GenericNew,
};

int
CfgMgr_define() {
    // PyType_Ready - finish type initialisation
    if (PyType_Ready(&CfgMgrType) < 0) {
        return (1);
    }
    Py_INCREF(&CfgMgrType);
    // REFCOUNT: PyModule_AddObject steals reference on success
    if (PyModule_AddObject(kea_module, "CfgMgr", (PyObject *) &CfgMgrType) < 0) {
        Py_DECREF(&CfgMgrType);
        return (1);
    }

    return (0);
}

}
