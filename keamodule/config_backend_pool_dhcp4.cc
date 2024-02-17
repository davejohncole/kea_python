#include "keamodule.h"

using namespace std;
using namespace isc::dhcp;
using namespace isc::db;
using namespace isc::data;

extern "C" {

static int
parse_backend_selector(const char *backend, BackendSelector& selector) {
    if (strcmp(backend, "mysql") == 0) {
        selector = BackendSelector(BackendSelector::Type::MYSQL);
        return (0);
    }
#ifdef HAVE_BACKEND_PGSQL
    if (strcmp(backend, "pgsql") == 0) {
        selector = BackendSelector(BackendSelector::Type::PGSQL);
        return (0);
    }
#else
    if (strcmp(backend, "postgresql") == 0) {
        selector = BackendSelector(BackendSelector::Type::POSTGRESQL);
        return (0);
    }
#endif
    if (strcmp(backend, "unspec") == 0) {
        selector = BackendSelector::UNSPEC();
        return (0);
    }
    PyErr_Format(PyExc_TypeError, "unrecognised selector: %s", backend);
    return (-1);
}

static int
parse_server_selector(PyObject *selector, ServerSelector& server_selector) {
    if (PyUnicode_Check(selector)) {
        PyObject *selector_utf8 = PyUnicode_AsUTF8String(selector);
        if (!selector_utf8) {
            return (-1);
        }
        const char *type = PyBytes_AsString(selector_utf8);
        try {
            if (strcmp(type, "unassigned") == 0) {
                server_selector = ServerSelector::UNASSIGNED();
            }
            else if (strcmp(type, "all") == 0) {
                server_selector = ServerSelector::ALL();
            }
            else if (strcmp(type, "any") == 0) {
                server_selector = ServerSelector::ANY();
            }
            else {
                server_selector = ServerSelector::ONE(string(type));
            }
        }
        catch (const exception &e) {
            Py_DECREF(selector_utf8);
            PyErr_SetString(PyExc_TypeError, e.what());
            return (-1);
        }
        Py_DECREF(selector_utf8);
        return (0);
    }
    if (PyList_Check(selector)) {
        std::set<string> tags;
        for (int i = 0; i < PyList_GET_SIZE(selector); i++) {
            PyObject *elem = PyList_GetItem(selector, i);
            if (!elem) {
                return (-1);
            }
            if (!PyUnicode_Check(elem)) {
                PyErr_Format(PyExc_TypeError, "expected string for server selector element %d", i);
                return (-1);
            }
            PyObject *selector_utf8 = PyUnicode_AsUTF8String(selector);
            if (!selector_utf8) {
                return (-1);
            }
            try {
                const char *selector = PyBytes_AsString(selector_utf8);
                tags.insert(selector);
                Py_DECREF(selector_utf8);
            }
            catch (const exception &e) {
                Py_DECREF(selector_utf8);
                PyErr_SetString(PyExc_TypeError, e.what());
                return (-1);
            }
        }
        server_selector = ServerSelector::MULTIPLE(tags);
        return (0);
    }
    PyErr_SetString(PyExc_TypeError, "server selector must be string or list of strings");
    return (-1);
}

static PyObject *
ConfigBackendPoolDHCPv4_createUpdateServer4(ConfigBackendPoolDHCPv4Object *self, PyObject *args) {
    const char *backend;
    ServerObject *server;

    if (!PyArg_ParseTuple(args, "sO!", &backend, &ServerType, &server)) {
        return (0);
    }
    BackendSelector backend_selector;
    if (parse_backend_selector(backend, backend_selector) < 0) {
        return (0);
    }

    try {
        self->ptr->createUpdateServer4(backend_selector, server->ptr);
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
ConfigBackendPoolDHCPv4_createUpdateSubnet4(ConfigBackendPoolDHCPv4Object *self, PyObject *args) {
    const char *backend;
    PyObject *selector;
    Subnet4Object *subnet;

    if (!PyArg_ParseTuple(args, "sOO!", &backend, &selector, &Subnet4Type, &subnet)) {
        return (0);
    }
    BackendSelector backend_selector;
    if (parse_backend_selector(backend, backend_selector) < 0) {
        return (0);
    }

    try {
        ServerSelector server_selector(ServerSelector::UNASSIGNED());
        if (parse_server_selector(selector, server_selector) < 0) {
            return (0);
        }
        self->ptr->createUpdateSubnet4(backend_selector, server_selector, subnet->ptr);
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
ConfigBackendPoolDHCPv4_deleteSubnet4(ConfigBackendPoolDHCPv4Object *self, PyObject *args) {
    const char *backend;
    PyObject *selector;
    uint32_t subnet_id;

    if (!PyArg_ParseTuple(args, "sOI", &backend, &selector, &subnet_id)) {
        return (0);
    }
    BackendSelector backend_selector;
    if (parse_backend_selector(backend, backend_selector) < 0) {
        return (0);
    }

    try {
        ServerSelector server_selector(ServerSelector::UNASSIGNED());
        if (parse_server_selector(selector, server_selector) < 0) {
            return (0);
        }
        uint64_t count = self->ptr->deleteSubnet4(backend_selector, server_selector, subnet_id);
        return (PyLong_FromLong(count));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
ConfigBackendPoolDHCPv4_deleteServer4(ConfigBackendPoolDHCPv4Object *self, PyObject *args) {
    const char *backend;
    const char *server_tag;

    if (!PyArg_ParseTuple(args, "ss", &backend, &server_tag)) {
        return (0);
    }
    BackendSelector backend_selector;
    if (parse_backend_selector(backend, backend_selector) < 0) {
        return (0);
    }

    try {
        uint64_t count = self->ptr->deleteServer4(backend_selector, ServerTag(server_tag));
        return (PyLong_FromLong(count));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
ConfigBackendPoolDHCPv4_getAllServers4(ConfigBackendPoolDHCPv4Object *self, PyObject *args) {
    const char *backend;

    if (!PyArg_ParseTuple(args, "s", &backend)) {
        return (0);
    }
    BackendSelector backend_selector;
    if (parse_backend_selector(backend, backend_selector) < 0) {
        return (0);
    }

    try {
        ServerCollection servers = self->ptr->getAllServers4(backend_selector);
        PyObject *list = PyList_New(servers.size());
        if (!list) {
            return (0);
        }
        try {
            int pos = 0;
            for (auto it = servers.begin(); it != servers.cend(); ++it, ++pos) {
                ServerPtr server = *it;
                PyObject *elem = Server_from_ptr(server);
                if (!elem || PyList_SetItem(list, pos, elem) < 0) {
                    Py_DECREF(list);
                    return (0);
                }
            }
        }
        catch (const exception &e) {
            PyErr_SetString(PyExc_TypeError, e.what());
            Py_DECREF(list);
            return (0);
        }
        return (list);
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
ConfigBackendPoolDHCPv4_getSubnet4(ConfigBackendPoolDHCPv4Object *self, PyObject *args) {
    const char *backend;
    PyObject *selector;
    uint32_t subnet_id;

    if (!PyArg_ParseTuple(args, "sOI", &backend, &selector, &subnet_id)) {
        return (0);
    }
    BackendSelector backend_selector;
    if (parse_backend_selector(backend, backend_selector) < 0) {
        return (0);
    }

    try {
        ServerSelector server_selector(ServerSelector::UNASSIGNED());
        if (parse_server_selector(selector, server_selector) < 0) {
            return (0);
        }
        Subnet4Ptr subnet = self->ptr->getSubnet4(backend_selector, server_selector, subnet_id);
        if (subnet) {
            return (Subnet4_from_ptr(subnet));
        }
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
ConfigBackendPoolDHCPv4_getAllSubnets4(ConfigBackendPoolDHCPv4Object *self, PyObject *args) {
    const char *backend;
    PyObject *selector;

    if (!PyArg_ParseTuple(args, "sO", &backend, &selector)) {
        return (0);
    }
    BackendSelector backend_selector;
    if (parse_backend_selector(backend, backend_selector) < 0) {
        return (0);
    }

    try {
        ServerSelector server_selector(ServerSelector::UNASSIGNED());
        if (parse_server_selector(selector, server_selector) < 0) {
            return (0);
        }
        Subnet4Collection subnets = self->ptr->getAllSubnets4(backend_selector, server_selector);
        PyObject *list = PyList_New(subnets.size());
        if (!list) {
            return (0);
        }
        try {
            int pos = 0;
            for (auto it = subnets.begin(); it != subnets.cend(); ++it, ++pos) {
                Subnet4Ptr subnet = *it;
                PyObject *elem = Subnet4_from_ptr(subnet);
                if (!elem || PyList_SetItem(list, pos, elem) < 0) {
                    Py_DECREF(list);
                    return (0);
                }
            }
        }
        catch (const exception &e) {
            PyErr_SetString(PyExc_TypeError, e.what());
            Py_DECREF(list);
            return (0);
        }
        return (list);
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

PyObject *
ConfigBackendPoolDHCPv4_from_ptr(ConfigBackendPoolDHCPv4Ptr &ptr) {
    ConfigBackendPoolDHCPv4Object *self = PyObject_New(ConfigBackendPoolDHCPv4Object, &ConfigBackendPoolDHCPv4Type);
    if (self) {
        new(&self->ptr) ConfigBackendPoolDHCPv4Ptr;
        self->ptr = ptr;
    }
    return ((PyObject *)self);
}

static PyMethodDef ConfigBackendPoolDHCPv4_methods[] = {
    {"createUpdateServer4", (PyCFunction) ConfigBackendPoolDHCPv4_createUpdateServer4, METH_VARARGS,
     "Creates or updates a server."},
    {"createUpdateSubnet4", (PyCFunction) ConfigBackendPoolDHCPv4_createUpdateSubnet4, METH_VARARGS,
     "Creates or updates a subnet."},
    {"deleteServer4", (PyCFunction) ConfigBackendPoolDHCPv4_deleteServer4, METH_VARARGS,
     "Deletes a server from the backend."},
    {"deleteSubnet4", (PyCFunction) ConfigBackendPoolDHCPv4_deleteSubnet4, METH_VARARGS,
     "Deletes subnet by identifier."},
    {"getAllServers4", (PyCFunction) ConfigBackendPoolDHCPv4_getAllServers4, METH_VARARGS,
     "Retrieves all servers from the particular backend."},
    {"getSubnet4", (PyCFunction) ConfigBackendPoolDHCPv4_getSubnet4, METH_VARARGS,
     "Retrieves a single subnet by subnet identifier."},
    {"getAllSubnets4", (PyCFunction) ConfigBackendPoolDHCPv4_getAllSubnets4, METH_VARARGS,
     "Collection of subnets or empty collection if no subnet found."},
    {0}  // Sentinel
};

static void
ConfigBackendPoolDHCPv4_dealloc(ConfigBackendPoolDHCPv4Object *self) {
    self->ptr.~ConfigBackendPoolDHCPv4Ptr();
    Py_TYPE(self)->tp_free((PyObject *) self);
}


static int
ConfigBackendPoolDHCPv4_init(ConfigBackendPoolDHCPv4Object *self, PyObject *args, PyObject *kwds) {
    PyErr_SetString(PyExc_RuntimeError, "cannot directly construct");
    return (-1);
}

PyTypeObject ConfigBackendPoolDHCPv4Type = {
    PyObject_HEAD_INIT(0)
    "kea.ConfigBackendPoolDHCPv4",              // tp_name
    sizeof(ConfigBackendPoolDHCPv4Object),      // tp_basicsize
    0,                                          // tp_itemsize
    (destructor) ConfigBackendPoolDHCPv4_dealloc,// tp_dealloc
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
    "Kea server ConfigBackendPoolDHCPv4",       // tp_doc
    0,                                          // tp_traverse
    0,                                          // tp_clear
    0,                                          // tp_richcompare
    0,                                          // tp_weaklistoffset
    0,                                          // tp_iter
    0,                                          // tp_iternext
    ConfigBackendPoolDHCPv4_methods,            // tp_methods
    0,                                          // tp_members
    0,                                          // tp_getset
    0,                                          // tp_base
    0,                                          // tp_dict
    0,                                          // tp_descr_get
    0,                                          // tp_descr_set
    0,                                          // tp_dictoffset
    (initproc) ConfigBackendPoolDHCPv4_init,    // tp_init
    PyType_GenericAlloc,                        // tp_alloc
    PyType_GenericNew                           // tp_new
};

int
ConfigBackendPoolDHCPv4_define() {
    if (PyType_Ready(&ConfigBackendPoolDHCPv4Type) < 0) {
        return (1);
    }
    Py_INCREF(&ConfigBackendPoolDHCPv4Type);
    if (PyModule_AddObject(kea_module, "ConfigBackendPoolDHCPv4", (PyObject *) &ConfigBackendPoolDHCPv4Type) < 0) {
        Py_DECREF(&ConfigBackendPoolDHCPv4Type);
        return (1);
    }

    return (0);
}

}
