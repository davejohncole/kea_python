#include "keamodule.h"

using namespace std;
using namespace isc::hooks;
using namespace isc::dhcp;
using namespace isc::data;
using namespace isc::util;
using namespace isc::asiolink;

extern "C" {

static PyObject *
Subnet4_delServerTag(Subnet4Object *self, PyObject *args) {
    const char *server_tag;

    if (!PyArg_ParseTuple(args, "s", &server_tag)) {
        return (0);
    }

    try {
        if (self->is_const) {
            PyErr_SetString(PyExc_TypeError, "not supported for const Subnet4");
            return (0);
        }
        self->ptr->delServerTag(server_tag);
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Subnet4_getID(Subnet4Object *self, PyObject *args) {
    try {
        SubnetID subnet_id = (self->is_const ? self->const_ptr : self->ptr)->getID();
        // REFCOUNT: PyLong_FromLong - returns new reference
        return (PyLong_FromLong(subnet_id));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Subnet4_getMetadata(Subnet4Object *self, PyObject *args) {
    try {
        ElementPtr ptr = (self->is_const ? self->const_ptr : self->ptr)->getMetadata();
        // REFCOUNT: element_to_object - returns new reference
        return (element_to_object(ptr));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Subnet4_getValid(Subnet4Object *self, PyObject *args) {
    try {
        uint32_t valid = (self->is_const ? self->const_ptr : self->ptr)->getValid();
        // REFCOUNT: PyLong_FromLong - returns new reference
        return (PyLong_FromLong(valid));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Subnet4_getServerTags(Subnet4Object *self, PyObject *args) {
    // REFCOUNT: PySet_New - returns new reference
    PyObject *tags = PySet_New(NULL);
    if (!tags) {
        return (0);
    }
    try {
        auto server_tags = (self->is_const ? self->const_ptr : self->ptr)->getServerTags();
        for (auto it = server_tags.begin(); it != server_tags.cend(); ++it) {
            // REFCOUNT: PyUnicode_FromString - returns new reference
            PyObject *elem = PyUnicode_FromString(it->get().c_str());
            // REFCOUNT: PySet_Add - reference neutral
            if (!elem || PySet_Add(tags, elem) < 0) {
                Py_DECREF(tags);
                Py_XDECREF(elem);
                return (0);
            }
            Py_DECREF(elem);
        }
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        Py_DECREF(tags);
        return (0);
    }
    return (tags);
}

static PyObject *
Subnet4_getSharedNetworkName(Subnet4Object *self, PyObject *args) {
    try {
        // REFCOUNT: PyUnicode_FromString - returns new reference
        return (PyUnicode_FromString((self->is_const ? self->const_ptr : self->ptr)->getSharedNetworkName().c_str()));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Subnet4_inRange(Subnet4Object *self, PyObject *args) {
    char *addr;

    if (!PyArg_ParseTuple(args, "s", &addr)) {
        return (0);
    }

    try {
        if ((self->is_const ? self->const_ptr : self->ptr)->inRange(IOAddress(string(addr)))) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Subnet4_setServerTag(Subnet4Object *self, PyObject *args) {
    const char *server_tag;

    if (!PyArg_ParseTuple(args, "s", &server_tag)) {
        return (0);
    }

    try {
        if (self->is_const) {
            PyErr_SetString(PyExc_TypeError, "not supported for const Subnet4");
            return (0);
        }
        self->ptr->setServerTag(server_tag);
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Subnet4_setSharedNetworkName(Subnet4Object *self, PyObject *args) {
    const char *name;

    if (!PyArg_ParseTuple(args, "s", &name)) {
        return (0);
    }

    try {
        if (self->is_const) {
            PyErr_SetString(PyExc_TypeError, "not supported for const Subnet4");
            return (0);
        }
        self->ptr->setSharedNetworkName(name);
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Subnet4_toText(Subnet4Object *self, PyObject *args) {
    try {
        string text = (self->is_const ? self->const_ptr : self->ptr)->toText();
        // REFCOUNT: PyUnicode_FromString - returns new reference
        return (PyUnicode_FromString(text.c_str()));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Subnet4_toElement(Subnet4Object *self, PyObject *args) {
    try {
        ElementPtr ptr = (self->is_const ? self->const_ptr : self->ptr)->toElement();
        // REFCOUNT: element_to_object - returns new reference
        return (element_to_object(ptr));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyMethodDef Subnet4_methods[] = {
    {"delServerTag", (PyCFunction) Subnet4_delServerTag, METH_VARARGS,
     "Deletes server tag. Remove the first occurrence of the given server tag."},
    {"getID", (PyCFunction) Subnet4_getID, METH_NOARGS,
     "Return unique ID for subnet."},
    {"getMetadata", (PyCFunction) Subnet4_getMetadata, METH_NOARGS,
     "Returns an object representing metadata to be returned with objects from the configuration backend."},
    {"getValid", (PyCFunction) Subnet4_getValid, METH_NOARGS,
     "Return unique ID for subnet."},
    {"getServerTags", (PyCFunction) Subnet4_getServerTags, METH_NOARGS,
     "Returns server tags."},
    {"getSharedNetworkName", (PyCFunction) Subnet4_getSharedNetworkName, METH_NOARGS,
     "Returns shared network name."},
    {"inRange", (PyCFunction) Subnet4_inRange, METH_VARARGS,
     "Checks if specified address is in range."},
    {"setServerTag", (PyCFunction) Subnet4_setServerTag, METH_VARARGS,
     "Adds new server tag."},
    {"setSharedNetworkName", (PyCFunction) Subnet4_setSharedNetworkName, METH_VARARGS,
     "Sets new shared network name."},
    {"toText", (PyCFunction) Subnet4_toText, METH_NOARGS,
     "Returns text representation of the subnet."},
    {"toElement", (PyCFunction) Subnet4_toElement, METH_NOARGS,
     "Unparse configuration object."},
    {0}  // Sentinel
};

static PyObject *
Subnet4_use_count(Subnet4Object *self, void *closure) {
    // REFCOUNT: PyLong_FromLong - returns new reference
    return (PyLong_FromLong(self->is_const ? self->const_ptr.use_count() : self->ptr.use_count()));
}

static PyGetSetDef Subnet4_getsetters[] = {
    {(char *)"use_count", (getter) Subnet4_use_count, (setter) 0, (char *)"shared_ptr use count", 0},
    {0}  // Sentinel
};

// tp_dealloc - called when refcount is zero
static void
Subnet4_dealloc(Subnet4Object *self) {
    self->ptr.~Subnet4Ptr();
    self->const_ptr.~ConstSubnet4Ptr();
    Py_TYPE(self)->tp_free((PyObject *) self);
}

// tp_init - called after tp_new has returned an instance
static int
Subnet4_init(Subnet4Object *self, PyObject *args, PyObject *kwds) {
    static const char *kwlist[] = {"prefix", "length", "t1", "t2", "valid_lifetime", "id", NULL};
    const char *prefix;
    uint8_t length;
    PyObject *t1_obj;
    PyObject *t2_obj;
    PyObject *valid_lifetime_obj;
    uint32_t subnet_id;

    new(&self->ptr) Subnet4Ptr;
    new(&self->const_ptr) ConstSubnet4Ptr;

    // REFCOUNT: PyArg_ParseTupleAndKeywords - returns borrowed references
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sbOOOI", (char **)kwlist,
         &prefix, &length, &t1_obj, &t2_obj, &valid_lifetime_obj, &subnet_id)) {
        return (-1);
    }

    try {
        Triplet<uint32_t> t1;
        Triplet<uint32_t> t2;
        Triplet<uint32_t> valid_lifetime;
        if (t1_obj != Py_None) {
            if (!PyLong_Check(t1_obj)) {
                PyErr_SetString(PyExc_TypeError, "'t1' must be int or None");
                return (-1);
            }
            t1 = PyLong_AsLong(t1_obj);
        }
        if (t2_obj != Py_None) {
            if (!PyLong_Check(t2_obj)) {
                PyErr_SetString(PyExc_TypeError, "'t2' must be int or None");
                return (-1);
            }
            t2 = PyLong_AsLong(t2_obj);
        }
        if (valid_lifetime_obj != Py_None) {
            if (!PyLong_Check(valid_lifetime_obj)) {
                PyErr_SetString(PyExc_TypeError, "'valid_lifetime' must be int or None");
                return (-1);
            }
            valid_lifetime = PyLong_AsLong(valid_lifetime_obj);
        }
        self->ptr.reset(new Subnet4(IOAddress(string(prefix)), length, t1, t2, valid_lifetime, SubnetID(subnet_id)));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (-1);
    }

    return (0);
}

// tp_new - allocate space and initialisation that can be repeated
static PyObject *
Subnet4_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    Subnet4Object *self;
    self = (Subnet4Object *) type->tp_alloc(type, 0);
    if (self) {
        new(&self->ptr) Subnet4Ptr;
        new(&self->const_ptr) ConstSubnet4Ptr;
    }
    return ((PyObject *) self);
}

PyTypeObject Subnet4Type = {
    .ob_base = PyObject_HEAD_INIT(0)
    .tp_name = "kea.Subnet4",
    .tp_basicsize = sizeof(Subnet4Object),
    .tp_dealloc = (destructor) Subnet4_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Kea server Subnet4"),
    .tp_methods = Subnet4_methods,
    .tp_getset = Subnet4_getsetters,
    .tp_init = (initproc) Subnet4_init,
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = Subnet4_new,
};

PyObject *
Subnet4_from_ptr(Subnet4Ptr &ptr) {
    // REFCOUNT: PyObject_New - returns new reference
    Subnet4Object *self = PyObject_New(Subnet4Object, &Subnet4Type);
    if (self) {
        new(&self->ptr) Subnet4Ptr;
        new(&self->const_ptr) ConstSubnet4Ptr;
        self->is_const = false;
        self->ptr = ptr;
    }
    return (PyObject *)self;
}

PyObject *
Subnet4_from_constptr(ConstSubnet4Ptr &ptr) {
    // REFCOUNT: PyObject_New - returns new reference
    Subnet4Object *self = PyObject_New(Subnet4Object, &Subnet4Type);
    if (self) {
        new(&self->ptr) Subnet4Ptr;
        new(&self->const_ptr) ConstSubnet4Ptr;
        self->is_const = true;
        self->const_ptr = ptr;
    }
    return (PyObject *)self;
}

int
Subnet4_define() {
    // PyType_Ready - finish type initialisation
    if (PyType_Ready(&Subnet4Type) < 0) {
        return (1);
    }
    Py_INCREF(&Subnet4Type);
    // REFCOUNT: PyModule_AddObject steals reference on success
    if (PyModule_AddObject(kea_module, "Subnet4", (PyObject *) &Subnet4Type) < 0) {
        Py_DECREF(&Subnet4Type);
        return (1);
    }

    return (0);
}

}
