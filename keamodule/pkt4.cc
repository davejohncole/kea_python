#include "keamodule.h"

using namespace std;
using namespace isc::hooks;
using namespace isc::dhcp;

extern "C" {

static PyObject *
Pkt4_getType(PyObject *self, PyObject *args) {
    try {
        uint8_t type = ((Pkt4Object *)self)->ptr->getType();
        return (PyLong_FromLong(type));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Pkt4_setType(PyObject *self, PyObject *args) {
    long type;

    if (!PyArg_ParseTuple(args, "i", &type)) {
        return (0);
    }

    try {
        ((Pkt4Object *)self)->ptr->setType(type);
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Pkt4_getLocalAddr(PyObject *self, PyObject *args) {
    try {
        string addr = ((Pkt4Object *)self)->ptr->getLocalAddr().toText();
        return (PyUnicode_FromString(addr.c_str()));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Pkt4_setLocalAddr(PyObject *self, PyObject *args) {
    char *addr;

    if (!PyArg_ParseTuple(args, "s", &addr)) {
        return (0);
    }

    try {
        ((Pkt4Object *)self)->ptr->setLocalAddr(isc::asiolink::IOAddress(string(addr)));
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Pkt4_getRemoteAddr(PyObject *self, PyObject *args) {
    try {
        string addr = ((Pkt4Object *)self)->ptr->getRemoteAddr().toText();
        return (PyUnicode_FromString(addr.c_str()));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Pkt4_setRemoteAddr(PyObject *self, PyObject *args) {
    char *addr;

    if (!PyArg_ParseTuple(args, "s", &addr)) {
        return (0);
    }

    try {
        ((Pkt4Object *)self)->ptr->setRemoteAddr(isc::asiolink::IOAddress(string(addr)));
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Pkt4_getYiaddr(PyObject *self, PyObject *args) {
    try {
        string addr = ((Pkt4Object *)self)->ptr->getYiaddr().toText();
        return (PyUnicode_FromString(addr.c_str()));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Pkt4_setYiaddr(PyObject *self, PyObject *args) {
    char *addr;

    if (!PyArg_ParseTuple(args, "s", &addr)) {
        return (0);
    }

    try {
        ((Pkt4Object *)self)->ptr->setYiaddr(isc::asiolink::IOAddress(string(addr)));
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Pkt4_getHWAddr(PyObject *self, PyObject *args) {
    try {
        string hwaddr = ((Pkt4Object *)self)->ptr->getHWAddr()->toText(false);
        return (PyUnicode_FromString(hwaddr.c_str()));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Pkt4_setHWAddr(PyObject *self, PyObject *args) {
    char *addr;

    if (!PyArg_ParseTuple(args, "s", &addr)) {
        return (0);
    }

    try {
        HWAddr hw = HWAddr::fromText(addr);
        ((Pkt4Object *)self)->ptr->setHWAddr(HWAddrPtr(new HWAddr(hw)));
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Pkt4_delOption(PyObject *self, PyObject *args) {
    long type;

    if (!PyArg_ParseTuple(args, "i", &type)) {
        return (0);
    }

    try {
        bool res = ((Pkt4Object *)self)->ptr->delOption(type);
        return (PyBool_FromLong(res));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Pkt4_addOption(PyObject *self, PyObject *args) {
    PyObject *opt;

    if (!PyArg_ParseTuple(args, "O", &opt)) {
        return (0);
    }
    if (!Option_Check(opt)) {
        PyErr_SetString(PyExc_TypeError, "opt must be instance of Option");
    }

    try {
        ((Pkt4Object *)self)->ptr->addOption(((OptionObject *)opt)->ptr);
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Pkt4_toText(PyObject *self, PyObject *args) {
    try {
        string addr = ((Pkt4Object *)self)->ptr->toText();
        return (PyUnicode_FromString(addr.c_str()));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
Pkt4_pack(PyObject *self, PyObject *args) {
    try {
        ((Pkt4Object *)self)->ptr->pack();
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyMethodDef Pkt4_methods[] = {
    {"getType", (PyCFunction) Pkt4_getType, METH_NOARGS,
     "For packets without DHCP Message Type option, it returns DHCP_NOTYPE (0)."},
    {"setType", (PyCFunction) Pkt4_setType, METH_VARARGS,
     "Sets DHCP message type."},
    {"getLocalAddr", (PyCFunction) Pkt4_getLocalAddr, METH_NOARGS,
     "Returns local IP address."},
    {"setLocalAddr", (PyCFunction) Pkt4_setLocalAddr, METH_VARARGS,
     "Sets local IP address."},
    {"getRemoteAddr", (PyCFunction) Pkt4_getRemoteAddr, METH_NOARGS,
     "Returns remote IP address."},
    {"setRemoteAddr", (PyCFunction) Pkt4_setRemoteAddr, METH_VARARGS,
     "Sets remote IP address."},
    {"getYiaddr", (PyCFunction) Pkt4_getYiaddr, METH_NOARGS,
     "Returns yiaddr field."},
    {"setYiaddr", (PyCFunction) Pkt4_setYiaddr, METH_VARARGS,
     "Sets yiaddr field."},
    {"getHWAddr", (PyCFunction) Pkt4_getHWAddr, METH_NOARGS,
     "Returns hardware address."},
    {"setHWAddr", (PyCFunction) Pkt4_setHWAddr, METH_VARARGS,
     "Sets hardware address."},
    {"delOption", (PyCFunction) Pkt4_delOption, METH_VARARGS,
     "Attempts to delete first suboption of requested type."},
    {"addOption", (PyCFunction) Pkt4_addOption, METH_VARARGS,
     "Adds an option to this packet."},
    {"toText", (PyCFunction) Pkt4_toText, METH_NOARGS,
     "Returns text representation of the packet."},
    {"pack", (PyCFunction) Pkt4_pack, METH_NOARGS,
     "Prepares on-wire format of DHCPv4 packet."},
    {0}  // Sentinel
};

static PyObject *
Pkt4_use_count(Pkt4Object *self, void *closure) {
    return (PyLong_FromLong(self->ptr.use_count()));
}

static PyGetSetDef Pkt4_getsetters[] = {
    {(char *)"use_count", (getter) Pkt4_use_count, (setter) 0, (char *)"shared_ptr use count", 0},
    {0}  // Sentinel
};

static void
Pkt4_dealloc(Pkt4Object *self) {
    self->ptr.reset();
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static int
Pkt4_init(Pkt4Object *self, PyObject *args, PyObject *kwds) {
    long msg_type;
    long transid;

    if (kwds != 0) {
        PyErr_SetString(PyExc_TypeError, "keyword arguments are not supported");
        return (0);
    }
    if (!PyArg_ParseTuple(args, "ii", &msg_type, &transid)) {
        return (-1);
    }

    self->ptr.reset(new Pkt4((int8_t)msg_type, (uint32_t)transid));

    return (0);
}

static PyObject *
Pkt4_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    Pkt4Object *self;
    self = (Pkt4Object *) type->tp_alloc(type, 0);
    if (self) {
        self->ptr.reset();
    }
    return ((PyObject *) self);
}

static PyTypeObject Pkt4Type = {
    PyObject_HEAD_INIT(0)
    "kea.Pkt4",                                 // tp_name
    sizeof(Pkt4Object),                         // tp_basicsize
    0,                                          // tp_itemsize
    (destructor) Pkt4_dealloc,                  // tp_dealloc
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
    "Kea server Pkt4",                          // tp_doc
    0,                                          // tp_traverse
    0,                                          // tp_clear
    0,                                          // tp_richcompare
    0,                                          // tp_weaklistoffset
    0,                                          // tp_iter
    0,                                          // tp_iternext
    Pkt4_methods,                               // tp_methods
    0,                                          // tp_members
    Pkt4_getsetters,                            // tp_getset
    0,                                          // tp_base
    0,                                          // tp_dict
    0,                                          // tp_descr_get
    0,                                          // tp_descr_set
    0,                                          // tp_dictoffset
    (initproc) Pkt4_init,                       // tp_init
    PyType_GenericAlloc,                        // tp_alloc
    Pkt4_new                                    // tp_new
};

int
Pkt4_Check(PyObject *object) {
    return (Py_TYPE(object) == &Pkt4Type);
}

PyObject *
Pkt4_from_handle(Pkt4Ptr &ptr) {
    Pkt4Object *self = PyObject_New(Pkt4Object, &Pkt4Type);
    if (self) {
        memset(&self->ptr, 0 , sizeof(self->ptr));
        self->ptr = ptr;
    }
    return (PyObject *)self;
}

int
Pkt4_define() {
    if (PyType_Ready(&Pkt4Type) < 0) {
        return (1);
    }
    Py_INCREF(&Pkt4Type);
    if (PyModule_AddObject(kea_module, "Pkt4", (PyObject *) &Pkt4Type) < 0) {
        Py_DECREF(&Pkt4Type);
        return (1);
    }

    return (0);
}

}
