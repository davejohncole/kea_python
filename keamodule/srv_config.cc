#include "keamodule.h"

using namespace std;
using namespace isc::hooks;
using namespace isc::dhcp;
using namespace isc::data;

extern "C" {

static PyObject *
SrvConfig_toElement(SrvConfigObject *self, PyObject *args) {
    try {
        ElementPtr ptr = self->ptr->toElement();
        // REFCOUNT: element_to_object - returns new reference
        return (element_to_object(ptr));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
SrvConfig_getCfgSubnets4(SrvConfigObject *self, PyObject *args) {
    try {
        CfgSubnets4Ptr ptr = self->ptr->getCfgSubnets4();
        // REFCOUNT: CfgSubnets4_from_ptr - returns new reference
        return (CfgSubnets4_from_ptr(ptr));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyMethodDef SrvConfig_methods[] = {
    {"toElement", (PyCFunction) SrvConfig_toElement, METH_NOARGS,
     "Unparse configuration object."},
    {"getCfgSubnets4", (PyCFunction) SrvConfig_getCfgSubnets4, METH_NOARGS,
     "Returns object holding subnets configuration for DHCPv4."},
    {0}  // Sentinel
};

static PyObject *
SrvConfig_use_count(OptionObject *self, void *closure) {
    // REFCOUNT: PyLong_FromLong - returns new reference
    return (PyLong_FromLong(self->ptr.use_count()));
}

static PyGetSetDef SrvConfig_getsetters[] = {
    {(char *)"use_count", (getter) SrvConfig_use_count, (setter) 0, (char *)"shared_ptr use count", 0},
    {0}  // Sentinel
};

// tp_dealloc - called when refcount is zero
static void
SrvConfig_dealloc(SrvConfigObject *self) {
    self->ptr.~SrvConfigPtr();
    Py_TYPE(self)->tp_free((PyObject *) self);
}

// tp_init - called after tp_new has returned an instance
static int
SrvConfig_init(SrvConfigObject *self, PyObject *args, PyObject *kwds) {
    new(&self->ptr) SrvConfigPtr;

    PyErr_SetString(PyExc_TypeError, "cannot create 'kea.SrvConfig' instances");
    return (-1);
}

// tp_new - allocate space and initialisation that can be repeated
static PyObject *
SrvConfig_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    SrvConfigObject *self;
    self = (SrvConfigObject *) type->tp_alloc(type, 0);
    if (self) {
        new(&self->ptr) SrvConfigPtr;
    }
    return ((PyObject *) self);
}

PyTypeObject SrvConfigType = {
    .ob_base = PyObject_HEAD_INIT(0)
    .tp_name = "kea.SrvConfig",
    .tp_basicsize = sizeof(SrvConfigObject),
    .tp_dealloc = (destructor) SrvConfig_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Kea server SrvConfig"),
    .tp_methods = SrvConfig_methods,
    .tp_getset = SrvConfig_getsetters,
    .tp_init = (initproc) SrvConfig_init,
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = SrvConfig_new,
};

PyObject *
SrvConfig_from_ptr(SrvConfigPtr &ptr) {
    // REFCOUNT: PyObject_New - returns new reference
    SrvConfigObject *self = PyObject_New(SrvConfigObject, &SrvConfigType);
    if (self) {
        new(&self->ptr) SrvConfigPtr;
        self->ptr = ptr;
    }
    return (PyObject *)self;
}

int
SrvConfig_define() {
    // PyType_Ready - finish type initialisation
    if (PyType_Ready(&SrvConfigType) < 0) {
        return (1);
    }
    Py_INCREF(&SrvConfigType);
    // REFCOUNT: PyModule_AddObject steals reference on success
    if (PyModule_AddObject(kea_module, "SrvConfig", (PyObject *) &SrvConfigType) < 0) {
        Py_DECREF(&SrvConfigType);
        return (1);
    }

    return (0);
}

}
