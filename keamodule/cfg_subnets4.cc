#include "keamodule.h"

using namespace std;
using namespace isc::hooks;
using namespace isc::dhcp;
using namespace isc::data;
using namespace isc::asiolink;

extern "C" {

static PyObject *
CfgSubnets4_add(CfgSubnets4Object *self, PyObject *args) {
    Subnet4Object *subnet;

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
    if (!PyArg_ParseTuple(args, "O!", &Subnet4Type, &subnet)) {
        return (0);
    }
    try {
        self->ptr->add(subnet->ptr);
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
CfgSubnets4_clear(CfgSubnets4Object *self, PyObject *args) {
    try {
        self->ptr->clear();
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
CfgSubnets4_delSubnetID(CfgSubnets4Object *self, PyObject *args) {
    uint32_t subnet_id;

    if (!PyArg_ParseTuple(args, "I", &subnet_id)) {
        return (0);
    }
    try {
        self->ptr->del(subnet_id);
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
CfgSubnets4_replace(CfgSubnets4Object *self, PyObject *args) {
    Subnet4Object *subnet;

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
    if (!PyArg_ParseTuple(args, "O!", &Subnet4Type, &subnet)) {
        return (0);
    }
    try {
        Subnet4Ptr ptr = self->ptr->replace(subnet->ptr);
        if (!ptr) {
            Py_RETURN_NONE;
        }
        // REFCOUNT: Subnet4_from_ptr - returns new reference
        return (Subnet4_from_ptr(ptr));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
CfgSubnets4_getAll(CfgSubnets4Object *self, PyObject *args) {
    try {
        const Subnet4Collection *all = self->ptr->getAll();
        if (!all) {
            Py_RETURN_NONE;
        }
        // REFCOUNT: PyList_New - returns new reference
        PyObject *list = PyList_New(0);
        if (!list) {
            return (0);
        }
        for (Subnet4Ptr ptr : *all) {
            // REFCOUNT: Subnet4_from_ptr - returns new reference
            PyObject *subnet = Subnet4_from_ptr(ptr);
            // REFCOUNT: PyList_Append - reference neutral
            if (!subnet || PyList_Append(list, subnet) < 0) {
                Py_DECREF(list);
                return (0);
            }
            Py_DECREF(subnet);
        }
        return (list);
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
CfgSubnets4_getSubnet(CfgSubnets4Object *self, PyObject *args) {
    unsigned long subnet_id;

    if (!PyArg_ParseTuple(args, "k", &subnet_id)) {
        return (0);
    }

    try {
        Subnet4Ptr ptr = self->ptr->getSubnet(subnet_id);
        if (!ptr) {
            Py_RETURN_NONE;
        }
        // REFCOUNT: Subnet4_from_ptr - returns new reference
        return (Subnet4_from_ptr(ptr));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
CfgSubnets4_selectSubnet(CfgSubnets4Object *self, PyObject *args) {
    char *addr;

    if (!PyArg_ParseTuple(args, "s", &addr)) {
        return (0);
    }

    try {
        Subnet4Ptr ptr = self->ptr->selectSubnet(IOAddress(string(addr)));
        if (!ptr) {
            Py_RETURN_NONE;
        }
        // REFCOUNT: Subnet4_from_ptr - returns new reference
        return (Subnet4_from_ptr(ptr));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
CfgSubnets4_toElement(CfgSubnets4Object *self, PyObject *args) {
    try {
        ElementPtr ptr = self->ptr->toElement();
        // element_to_object - returns new reference
        return (element_to_object(ptr));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyMethodDef CfgSubnets4_methods[] = {
    {"add", (PyCFunction) CfgSubnets4_add, METH_VARARGS,
     "Adds new subnet to the configuration."},
    {"clear", (PyCFunction) CfgSubnets4_clear, METH_NOARGS,
     "Clears all subnets from the configuration."},
    {"delSubnetID", (PyCFunction) CfgSubnets4_delSubnetID, METH_VARARGS,
     "Removes subnet from the configuration."},
    {"replace", (PyCFunction) CfgSubnets4_replace, METH_VARARGS,
     "Replaces subnet in the configuration. This method replaces a subnet by another subnet with the same ID."
     " The prefix should be the same too."},
    {"getAll", (PyCFunction) CfgSubnets4_getAll, METH_NOARGS,
     "Returns collection of all IPv4 subnets."},
    {"getSubnet", (PyCFunction) CfgSubnets4_getSubnet, METH_VARARGS,
     "Returns subnet with specified subnet-id value."},
    {"selectSubnet", (PyCFunction) CfgSubnets4_selectSubnet, METH_VARARGS,
     "Returns a pointer to the selected subnet."},
    {"toElement", (PyCFunction) CfgSubnets4_toElement, METH_NOARGS,
     "Unparse configuration object."},
    {0}  // Sentinel
};

static PyObject *
CfgSubnets4_use_count(OptionObject *self, void *closure) {
    // REFCOUNT: PyLong_FromLong - returns new reference
    return (PyLong_FromLong(self->ptr.use_count()));
}

static PyGetSetDef CfgSubnets4_getsetters[] = {
    {(char *)"use_count", (getter) CfgSubnets4_use_count, (setter) 0, (char *)"shared_ptr use count", 0},
    {0}  // Sentinel
};

// tp_dealloc - called when refcount is zero
static void
CfgSubnets4_dealloc(CfgSubnets4Object *self) {
    self->ptr.~CfgSubnets4Ptr();
    Py_TYPE(self)->tp_free((PyObject *) self);
}

// tp_init - called after tp_new has returned an instance
static int
CfgSubnets4_init(CfgSubnets4Object *self, PyObject *args, PyObject *kwds) {
    new(&self->ptr) CfgSubnets4Ptr;

    PyErr_SetString(PyExc_TypeError, "cannot create 'kea.CfgSubnets4' instances");
    return (-1);
}

// tp_new - allocate space and initialisation that can be repeated
static PyObject *
CfgSubnets4_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    CfgSubnets4Object *self;
    self = (CfgSubnets4Object *) type->tp_alloc(type, 0);
    if (self) {
        new(&self->ptr) CfgSubnets4Ptr;
    }
    return ((PyObject *) self);
}

PyTypeObject CfgSubnets4Type = {
    .ob_base = PyObject_HEAD_INIT(0)
    .tp_name = "kea.CfgSubnets4",
    .tp_basicsize = sizeof(CfgSubnets4Object),
    .tp_dealloc = (destructor) CfgSubnets4_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Kea server CfgSubnets4"),
    .tp_methods = CfgSubnets4_methods,
    .tp_getset = CfgSubnets4_getsetters,
    .tp_init = (initproc) CfgSubnets4_init,
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = CfgSubnets4_new,
};

PyObject *
CfgSubnets4_from_ptr(CfgSubnets4Ptr &ptr) {
    // REFCOUNT: PyObject_New - returns new reference
    CfgSubnets4Object *self = PyObject_New(CfgSubnets4Object, &CfgSubnets4Type);
    if (self) {
        new(&self->ptr) CfgSubnets4Ptr;
        self->ptr = ptr;
    }
    return ((PyObject *)self);
}

int
CfgSubnets4_define() {
    // PyType_Ready - finish type initialisation
    if (PyType_Ready(&CfgSubnets4Type) < 0) {
        return (1);
    }
    Py_INCREF(&CfgSubnets4Type);
    // REFCOUNT: PyModule_AddObject steals reference on success
    if (PyModule_AddObject(kea_module, "CfgSubnets4", (PyObject *) &CfgSubnets4Type) < 0) {
        Py_DECREF(&CfgSubnets4Type);
        return (1);
    }

    return (0);
}

}
