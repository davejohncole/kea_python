#include "keamodule.h"

using namespace std;
using namespace isc::dhcp;
using namespace isc::asiolink;
using namespace isc::util;

extern "C" {

// need BaseHostDataSource

static PyObject *
HostMgr_add(HostMgrObject *self, PyObject *args) {
    HostObject *host;

    if (!PyArg_ParseTuple(args, "O!", &HostType, &host)) {
        return (0);
    }

    try {
        self->mgr->add(host->ptr);
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
HostMgr_get(HostMgrObject *self, PyObject *args) {
    unsigned long subnet_id;
    const char *ip_address = 0;
    const char *identifier_type = 0;
    const char *identifier = 0;

    if (PyTuple_GET_SIZE(args) == 2) {
        if (!PyArg_ParseTuple(args, "ks", &subnet_id, &ip_address)) {
            return (0);
        }
    } else {
        if (!PyArg_ParseTuple(args, "kss", &subnet_id, &identifier_type, &identifier)) {
            return (0);
        }
    }

    try {
        ConstHostPtr host;
        if (ip_address != 0) {
            host = self->mgr->get4(subnet_id, IOAddress(ip_address));
        } else {
            std::vector<uint8_t> binary = str::quotedStringToBinary(identifier);
            if (binary.empty()) {
                str::decodeFormattedHexString(identifier, binary);
            }
            host = self->mgr->get4(subnet_id, Host::getIdentifierType(identifier_type), &binary.front(), binary.size());
        }
        if (!host.get()) {
            Py_RETURN_NONE;
        }
        return Host_from_constptr(host);
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
collection_to_list(ConstHostCollection& hosts) {
    PyObject *result = PyList_New(hosts.size());
    if (result == 0) {
        return 0;
    }
    int pos = 0;
    for (auto host : hosts) {
        PyObject *pyhost = Host_from_constptr(host);
        if (pyhost == 0) {
            Py_DECREF(result);
            return (0);
        }
        if (PyList_SetItem(result, pos++, pyhost) < 0) {
            Py_DECREF(result);
            return (0);
        }
    }
    return (result);
}

static PyObject *
HostMgr_getAll4(HostMgrObject *self, PyObject *args) {
    unsigned long subnet_id;

    if (!PyArg_ParseTuple(args, "k", &subnet_id)) {
        return (0);
    }

    try {
        ConstHostCollection hosts = self->mgr->getAll4(subnet_id);
        return (collection_to_list(hosts));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
HostMgr_getPage4(HostMgrObject *self, PyObject *args) {
    unsigned long subnet_id;
    unsigned long source_index = 0;
    uint64_t lower_host_id = 0;
    unsigned long page_size = 0;

    if (!PyArg_ParseTuple(args, "kkKk", &subnet_id, &source_index, &lower_host_id, &page_size)) {
        return (0);
    }

    try {
        HostPageSize host_page_size(page_size);
        ConstHostCollection hosts = self->mgr->getPage4(subnet_id, source_index, lower_host_id, host_page_size);
        PyObject *host_list = collection_to_list(hosts);
        if (host_list == 0) {
            return (0);
        }
        PyObject *result = Py_BuildValue("Ok", host_list, source_index);
        Py_DECREF(host_list);
        return (result);
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
HostMgr_del_(HostMgrObject *self, PyObject *args) {
    unsigned long subnet_id;
    const char *ip_address;

    if (!PyArg_ParseTuple(args, "ks", &subnet_id, &ip_address)) {
        return (0);
    }

    try {
        if (self->mgr->del(subnet_id, IOAddress(ip_address))) {
            Py_RETURN_TRUE;
        } else {
            Py_RETURN_FALSE;
        }
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
HostMgr_del4(HostMgrObject *self, PyObject *args) {
    unsigned long subnet_id;
    const char *identifier_type;
    const char *identifier;

    if (!PyArg_ParseTuple(args, "kss", &subnet_id, &identifier_type, &identifier)) {
        return (0);
    }

    try {
        std::vector<uint8_t> binary = str::quotedStringToBinary(identifier);
        if (binary.empty()) {
            str::decodeFormattedHexString(identifier, binary);
        }
        if (self->mgr->del4(subnet_id, Host::getIdentifierType(identifier_type), &binary.front(), binary.size())) {
            Py_RETURN_TRUE;
        } else {
            Py_RETURN_FALSE;
        }
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

PyObject *
HostMgr_from_ptr(HostMgr *mgr) {
    HostMgrObject *self = PyObject_New(HostMgrObject, &HostMgrType);
    if (self) {
        self->mgr = mgr;
    }
    return (PyObject *)self;
}

static PyObject *
HostMgr_instance(HostMgrObject *self, PyObject *args) {
    try {
        HostMgr &mgr = HostMgr::instance();
        return (HostMgr_from_ptr(&mgr));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}


static PyMethodDef HostMgr_methods[] = {
    {"instance", (PyCFunction) HostMgr_instance, METH_NOARGS|METH_STATIC,
     "Returns a sole instance of the HostMgr."},
    {"add", (PyCFunction) HostMgr_add, METH_VARARGS,
     "Adds a new host to the alternate data source."},
    {"get", (PyCFunction) HostMgr_get, METH_VARARGS,
     "Returns a host connected to the IPv4 subnet."},
    {"getAll4", (PyCFunction) HostMgr_getAll4, METH_VARARGS,
     "Return all hosts in a DHCPv4 subnet."},
    {"getPage4", (PyCFunction) HostMgr_getPage4, METH_VARARGS,
     "Returns range of hosts in a DHCPv4 subnet."},
    {"del_", (PyCFunction) HostMgr_del_, METH_VARARGS,
     "Attempts to delete a host by address."},
    {"del4", (PyCFunction) HostMgr_del4, METH_VARARGS,
     "Attempts to delete a host by (subnet4-id, identifier, identifier-type)."},
    {0}  // Sentinel
};

static int
HostMgr_init(HostMgrObject *self, PyObject *args, PyObject *kwds) {
    PyErr_SetString(PyExc_RuntimeError, "cannot directly construct");
    return (-1);
}

static PyObject *
HostMgr_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    HostMgrObject *self;
    self = (HostMgrObject *) type->tp_alloc(type, 0);
    return ((PyObject *) self);
}

PyTypeObject HostMgrType = {
    PyObject_HEAD_INIT(0)
    "kea.HostMgr",                              // tp_name
    sizeof(HostMgrObject),                      // tp_basicsize
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
    "Kea server HostMgr",                       // tp_doc
    0,                                          // tp_traverse
    0,                                          // tp_clear
    0,                                          // tp_richcompare
    0,                                          // tp_weaklistoffset
    0,                                          // tp_iter
    0,                                          // tp_iternext
    HostMgr_methods,                            // tp_methods
    0,                                          // tp_members
    0,                                          // tp_getset
    0,                                          // tp_base
    0,                                          // tp_dict
    0,                                          // tp_descr_get
    0,                                          // tp_descr_set
    0,                                          // tp_dictoffset
    (initproc) HostMgr_init,                    // tp_init
    PyType_GenericAlloc,                        // tp_alloc
    HostMgr_new                                 // tp_new
};

int
HostMgr_define() {
    if (PyType_Ready(&HostMgrType) < 0) {
        return (1);
    }
    Py_INCREF(&HostMgrType);
    if (PyModule_AddObject(kea_module, "HostMgr", (PyObject *) &HostMgrType) < 0) {
        Py_DECREF(&HostMgrType);
        return (1);
    }

    return (0);
}

}
