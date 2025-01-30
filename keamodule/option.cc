#include "keamodule.h"

using namespace std;
using namespace isc::hooks;
using namespace isc::dhcp;
using namespace isc::util;

extern "C" {

static PyObject *
Option_getType(OptionObject *self, PyObject *args) {
    try {
        // REFCOUNT: PyLong_FromLong - returns new reference
        return (PyLong_FromLong(self->ptr->getType()));
    } catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Option_getBytes(OptionObject *self, PyObject *args) {
    try {
        vector<uint8_t> value = self->ptr->toBinary();
        // REFCOUNT: PyBytes_FromStringAndSize - returns new reference
        return (PyBytes_FromStringAndSize((const char *) &value[0], value.size()));
    } catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Option_setBytes(OptionObject *self, PyObject *args) {
    PyObject *data;

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
    if (!PyArg_ParseTuple(args, "S", &data)) {
        return (0);
    }

    try {
        char *buff;
        Py_ssize_t len;

        PyBytes_AsStringAndSize(data, &buff, &len);
        self->ptr->setData(&buff[0], &buff[len]);
        // REFCOUNT: return new reference to self
        Py_INCREF(self);
        return ((PyObject *)self);
    } catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Option_getString(OptionObject *self, PyObject *args) {
    try {
        vector<uint8_t> value = self->ptr->toBinary();
        // REFCOUNT: PyUnicode_FromStringAndSize - returns new reference
        return (PyUnicode_FromStringAndSize((const char *) &value[0], value.size()));
    } catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Option_setString(OptionObject *self, PyObject *args) {
    char *data;
    Py_ssize_t len;

    if (!PyArg_ParseTuple(args, "s#", &data, &len)) {
        return (0);
    }

    try {
        self->ptr->setData(&data[0], &data[len]);
        // REFCOUNT: return new reference to self
        Py_INCREF(self);
        return ((PyObject *)self);
    } catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Option_getUint8(OptionObject *self, PyObject *args) {
    try {
        // REFCOUNT: PyLong_FromLong - returns new reference
        return (PyLong_FromLong(self->ptr->getUint8()));
    } catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Option_setUint8(OptionObject *self, PyObject *args) {
    unsigned char value;

    if (!PyArg_ParseTuple(args, "b", &value)) {
        return (0);
    }

    try {
        self->ptr->setUint8(value);
        // REFCOUNT: return new reference to self
        Py_INCREF(self);
        return ((PyObject *)self);
    } catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Option_getUint16(OptionObject *self, PyObject *args) {
    try {
        // REFCOUNT: PyLong_FromLong - returns new reference
        return (PyLong_FromLong(self->ptr->getUint16()));
    } catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Option_setUint16(OptionObject *self, PyObject *args) {
    unsigned short value;

    if (!PyArg_ParseTuple(args, "H", &value)) {
        return (0);
    }

    try {
        self->ptr->setUint16(value);
        // REFCOUNT: return new reference to self
        Py_INCREF(self);
        return ((PyObject *)self);
    } catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Option_getUint32(OptionObject *self, PyObject *args) {
    try {
        // REFCOUNT: PyLong_FromLong - returns new reference
        return (PyLong_FromLong(self->ptr->getUint32()));
    } catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Option_setUint32(OptionObject *self, PyObject *args) {
    unsigned long value;

    if (!PyArg_ParseTuple(args, "k", &value)) {
        return (0);
    }

    try {
        self->ptr->setUint32(value);
        // REFCOUNT: return new reference to self
        Py_INCREF(self);
        return ((PyObject *)self);
    } catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Option_addOption(OptionObject *self, PyObject *args) {
    PyObject *value;

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
    if (!PyArg_ParseTuple(args, "O!", &OptionType, &value)) {
        return (0);
    }

    try {
        self->ptr->addOption(((OptionObject *)value)->ptr);
        // REFCOUNT: return new reference to self
        Py_INCREF(self);
        return ((PyObject *)self);
    } catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Option_getOption(OptionObject *self, PyObject *args) {
    unsigned int type;

    if (!PyArg_ParseTuple(args, "H", &type)) {
        return (0);
    }

    try {
        OptionPtr ptr = self->ptr->getOption(type);
        if (ptr) {
            // REFCOUNT: Option_from_ptr - returns new reference
            return Option_from_ptr(ptr);
        }
        Py_RETURN_NONE;
    } catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Option_pack(OptionObject *self, PyObject *args) {
    try {
        OutputBuffer buf(64);
        self->ptr->pack(buf);
        // REFCOUNT: PyBytes_FromStringAndSize - returns new reference
        return (PyBytes_FromStringAndSize((char *)buf.getData(), buf.getLength()));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Option_toText(OptionObject *self, PyObject *args) {
    try {
        string addr = self->ptr->toText();
        // REFCOUNT: PyUnicode_FromString - returns new reference
        return (PyUnicode_FromString(addr.c_str()));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyMethodDef Option_methods[] = {
    {"getType", (PyCFunction) Option_getType, METH_NOARGS,
     "Returns option type."},
    {"getBytes", (PyCFunction) Option_getBytes, METH_NOARGS,
     "Returns option data."},
    {"setBytes", (PyCFunction) Option_setBytes, METH_VARARGS,
     "Sets content to data."},
    {"getString", (PyCFunction) Option_getString, METH_NOARGS,
     "Returns option data from UTF-8 decoded string."},
    {"setString", (PyCFunction) Option_setString, METH_VARARGS,
     "Sets content to UTF-8 encoded string."},
    {"getUint8", (PyCFunction) Option_getUint8, METH_NOARGS,
     "Returns content of first byte."},
    {"setUint8", (PyCFunction) Option_setUint8, METH_VARARGS,
     "Sets content to single uint8 value."},
    {"getUint16", (PyCFunction) Option_getUint16, METH_NOARGS,
     "Returns content of first word."},
    {"setUint16", (PyCFunction) Option_setUint16, METH_VARARGS,
     "Sets content to single uint16 value."},
    {"getUint32", (PyCFunction) Option_getUint32, METH_NOARGS,
     "Returns content of first double word."},
    {"setUint32", (PyCFunction) Option_setUint32, METH_VARARGS,
     "Sets content to single uint32 value."},
    {"addOption", (PyCFunction) Option_addOption, METH_VARARGS,
     "Adds a sub-option."},
    {"getOption", (PyCFunction) Option_getOption, METH_VARARGS,
     "Returns suboption of specific type."},
    {"pack", (PyCFunction) Option_pack, METH_NOARGS,
     "Return option in wire-format."},
    {"toText", (PyCFunction) Option_toText, METH_NOARGS,
     "Returns string representation of the option."},
    {0}  // Sentinel
};

static PyObject *
Option_use_count(OptionObject *self, void *closure) {
    // REFCOUNT: PyLong_FromLong - returns new reference
    return (PyLong_FromLong(self->ptr.use_count()));
}

static PyGetSetDef Option_getsetters[] = {
    {(char *)"use_count", (getter) Option_use_count, (setter) 0, (char *)"shared_ptr use count", 0},
    {0}  // Sentinel
};

// tp_dealloc - called when refcount is zero
static void
Option_dealloc(OptionObject *self) {
    self->ptr.~OptionPtr();
    Py_TYPE(self)->tp_free((PyObject *) self);
}

// tp_init - called after tp_new has returned an instance
static int
Option_init(OptionObject *self, PyObject *args, PyObject *kwds) {
    unsigned short type;

    new(&self->ptr) OptionPtr;

    if (kwds != 0) {
        PyErr_SetString(PyExc_TypeError, "keyword arguments are not supported");
        return (-1);
    }
    if (!PyArg_ParseTuple(args, "H", &type)) {
        return (-1);
    }
    self->ptr.reset(new Option(Option::Universe::V4, type));

    return (0);
}

// tp_new - allocate space and initialisation that can be repeated
static PyObject *
Option_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    OptionObject *self;
    self = (OptionObject *) type->tp_alloc(type, 0);
    if (self) {
        new(&self->ptr) OptionPtr;
    }
    return ((PyObject *) self);
}

PyTypeObject OptionType = {
    .ob_base = PyObject_HEAD_INIT(0)
    .tp_name = "kea.Option",
    .tp_basicsize = sizeof(OptionObject),
    .tp_dealloc = (destructor) Option_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Kea server Option"),
    .tp_methods = Option_methods,
    .tp_getset = Option_getsetters,
    .tp_init = (initproc) Option_init,
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = Option_new,
};

PyObject *
Option_from_ptr(OptionPtr &ptr) {
    // REFCOUNT: PyObject_New - returns new reference
    OptionObject *self = PyObject_New(OptionObject, &OptionType);
    if (self) {
        new(&self->ptr) OptionPtr;
        self->ptr = ptr;
    }
    return (PyObject *)self;
}

int
Option_define() {
    // PyType_Ready - finish type initialisation
    if (PyType_Ready(&OptionType) < 0) {
        return (1);
    }
    Py_INCREF(&OptionType);
    // REFCOUNT: PyModule_AddObject steals reference on success
    if (PyModule_AddObject(kea_module, "Option", (PyObject *) &OptionType) < 0) {
        Py_DECREF(&OptionType);
        return (1);
    }

    return (0);
}

}
