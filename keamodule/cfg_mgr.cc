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
    PyErr_SetString(PyExc_RuntimeError, "cannot directly construct");
    return (-1);
}

PyTypeObject CfgMgrType = {
    PyObject_HEAD_INIT(0)
    "kea.CfgMgr",                               // tp_name
    sizeof(CfgMgrObject),                       // tp_basicsize
    0,                                          // tp_itemsize
    (destructor) CfgMgr_dealloc,                // tp_dealloc
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
    "Kea server CfgMgr",                        // tp_doc
    0,                                          // tp_traverse
    0,                                          // tp_clear
    0,                                          // tp_richcompare
    0,                                          // tp_weaklistoffset
    0,                                          // tp_iter
    0,                                          // tp_iternext
    CfgMgr_methods,                             // tp_methods
    0,                                          // tp_members
    0,                                          // tp_getset
    0,                                          // tp_base
    0,                                          // tp_dict
    0,                                          // tp_descr_get
    0,                                          // tp_descr_set
    0,                                          // tp_dictoffset
    (initproc) CfgMgr_init,                     // tp_init
    PyType_GenericAlloc,                        // tp_alloc
    PyType_GenericNew                           // tp_new
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
