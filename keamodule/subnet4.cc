#include "keamodule.h"

using namespace std;
using namespace isc::hooks;
using namespace isc::dhcp;
using namespace isc::data;

extern "C" {

static PyObject *
Subnet4_getID(Subnet4Object *self, PyObject *args) {
    try {
        SubnetID subnet_id = self->ptr->getID();
        return (PyLong_FromLong(subnet_id));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Subnet4_toElement(Subnet4Object *self, PyObject *args) {
    try {
        ElementPtr ptr = self->ptr->toElement();
        return (element_to_object(ptr));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyMethodDef Subnet4_methods[] = {
    {"getID", (PyCFunction) Subnet4_getID, METH_NOARGS,
     "Return unique ID for subnet."},
    {"toElement", (PyCFunction) Subnet4_toElement, METH_NOARGS,
     "Unparse configuration object."},
    {0}  // Sentinel
};

static PyObject *
Subnet4_use_count(OptionObject *self, void *closure) {
    return (PyLong_FromLong(self->ptr.use_count()));
}

static PyGetSetDef Subnet4_getsetters[] = {
    {(char *)"use_count", (getter) Subnet4_use_count, (setter) 0, (char *)"shared_ptr use count", 0},
    {0}  // Sentinel
};

static void
Subnet4_dealloc(Subnet4Object *self) {
    self->ptr.reset();
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *
Subnet4_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    Subnet4Object *self;
    self = (Subnet4Object *) type->tp_alloc(type, 0);
    if (self) {
        self->ptr.reset();
    }
    return ((PyObject *) self);
}

PyTypeObject Subnet4Type = {
    PyObject_HEAD_INIT(0)
    "kea.Subnet4",                              // tp_name
    sizeof(Subnet4Object),                      // tp_basicsize
    0,                                          // tp_itemsize
    (destructor) Subnet4_dealloc,               // tp_dealloc
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
    "Kea server Subnet4",                       // tp_doc
    0,                                          // tp_traverse
    0,                                          // tp_clear
    0,                                          // tp_richcompare
    0,                                          // tp_weaklistoffset
    0,                                          // tp_iter
    0,                                          // tp_iternext
    Subnet4_methods,                            // tp_methods
    0,                                          // tp_members
    Subnet4_getsetters,                         // tp_getset
    0,                                          // tp_base
    0,                                          // tp_dict
    0,                                          // tp_descr_get
    0,                                          // tp_descr_set
    0,                                          // tp_dictoffset
    0,                                          // tp_init
    PyType_GenericAlloc,                        // tp_alloc
    Subnet4_new                                 // tp_new
};

PyObject *
Subnet4_from_ptr(Subnet4Ptr &ptr) {
    Subnet4Object *self = PyObject_New(Subnet4Object, &Subnet4Type);
    if (self) {
        memset(&self->ptr, 0 , sizeof(self->ptr));
        self->ptr = ptr;
    }
    return (PyObject *)self;
}

int
Subnet4_define() {
    if (PyType_Ready(&Subnet4Type) < 0) {
        return (1);
    }
    Py_INCREF(&Subnet4Type);
    if (PyModule_AddObject(kea_module, "Subnet4", (PyObject *) &Subnet4Type) < 0) {
        Py_DECREF(&Subnet4Type);
        return (1);
    }

    return (0);
}

}
