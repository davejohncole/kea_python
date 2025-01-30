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

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
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
        // REFCOUNT: Host_from_constptr - returns new reference
        return Host_from_constptr(host);
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
collection_to_list(ConstHostCollection& hosts) {
    // REFCOUNT: PyList_New - returns new reference
    PyObject *result = PyList_New(hosts.size());
    if (result == 0) {
        return 0;
    }
    int pos = 0;
    for (auto host : hosts) {
        // REFCOUNT: Host_from_constptr - returns new reference
        PyObject *pyhost = Host_from_constptr(host);
        if (pyhost == 0) {
            Py_DECREF(result);
            return (0);
        }
        // REFCOUNT: PyList_SetItem - steals reference
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
        // REFCOUNT: collection_to_list - returns new reference
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
        // REFCOUNT: collection_to_list - returns new reference
        PyObject *host_list = collection_to_list(hosts);
        if (host_list == 0) {
            return (0);
        }
        // REFCOUNT: Py_BuildValue - returns new reference - reference neutral
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
    // REFCOUNT: PyObject_New - returns new reference
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
        // REFCOUNT: HostMgr_from_ptr - returns new reference
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

// tp_init - called after tp_new has returned an instance
static int
HostMgr_init(HostMgrObject *self, PyObject *args, PyObject *kwds) {
    PyErr_SetString(PyExc_TypeError, "cannot create 'kea.HostMgr' instances");
    return (-1);
}

PyTypeObject HostMgrType = {
    .ob_base = PyObject_HEAD_INIT(0)
    .tp_name = "kea.HostMgr",
    .tp_basicsize = sizeof(HostMgrObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Kea server HostMgr"),
    .tp_methods = HostMgr_methods,
    .tp_init = (initproc) HostMgr_init,
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = PyType_GenericNew,
};

int
HostMgr_define() {
    // PyType_Ready - finish type initialisation
    if (PyType_Ready(&HostMgrType) < 0) {
        return (1);
    }
    Py_INCREF(&HostMgrType);
    // REFCOUNT: PyModule_AddObject steals reference on success
    if (PyModule_AddObject(kea_module, "HostMgr", (PyObject *) &HostMgrType) < 0) {
        Py_DECREF(&HostMgrType);
        return (1);
    }

    return (0);
}

}
