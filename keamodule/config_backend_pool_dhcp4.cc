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
        // PyUnicode_AsUTF8String - returns new reference with UTF-8 encoding of str - buffer cached in str
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
            // PyList_GetItem - returns borrowed reference
            PyObject *elem = PyList_GetItem(selector, i);
            if (!elem) {
                return (-1);
            }
            if (!PyUnicode_Check(elem)) {
                PyErr_Format(PyExc_TypeError, "expected string for server selector element %d", i);
                return (-1);
            }
            // PyUnicode_AsUTF8String - returns new reference with UTF-8 encoding of str - buffer cached in str
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

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
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
ConfigBackendPoolDHCPv4_createUpdateClientClass4(ConfigBackendPoolDHCPv4Object *self, PyObject *args) {
    const char *backend;
    PyObject *selector;
    ClientClassDefObject *client_class;
    const char *follow_class_name = "";

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
    if (!PyArg_ParseTuple(args, "sOO!|s", &backend, &selector, &ClientClassDefType, &client_class, &follow_class_name)) {
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
        self->ptr->createUpdateClientClass4(backend_selector, server_selector, client_class->ptr, std::string(follow_class_name));
        // REFCOUNT: Py_RETURN_NONE - returns None
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        // REFCOUNT: PyErr_SetString - does not affect reference counts
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
ConfigBackendPoolDHCPv4_createUpdateSubnet4(ConfigBackendPoolDHCPv4Object *self, PyObject *args) {
    const char *backend;
    PyObject *selector;
    Subnet4Object *subnet;

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
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

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
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
ConfigBackendPoolDHCPv4_deleteClientClass4(ConfigBackendPoolDHCPv4Object *self, PyObject *args) {
    const char *backend;
    PyObject *selector;
    const char *name;

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
    if (!PyArg_ParseTuple(args, "sOs", &backend, &selector, &name)) {
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
        uint64_t count = self->ptr->deleteClientClass4(backend_selector, server_selector, std::string(name));
        // REFCOUNT: PyLong_FromLong - returns new reference
        return (PyLong_FromLong(count));
    }
    catch (const exception &e) {
        // REFCOUNT: PyErr_SetString - does not affect reference counts
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
ConfigBackendPoolDHCPv4_getClientClass4(ConfigBackendPoolDHCPv4Object *self, PyObject *args) {
    const char *backend;
    PyObject *selector;
    const char *name;

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
    if (!PyArg_ParseTuple(args, "sOs", &backend, &selector, &name)) {
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
        ClientClassDefPtr client_class = self->ptr->getClientClass4(backend_selector, server_selector, std::string(name));
        if (client_class) {
            // REFCOUNT: ClientClassDef_from_ptr - returns new reference
            return ClientClassDef_from_ptr(client_class);
        }
        // REFCOUNT: Py_RETURN_NONE - returns None
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        // REFCOUNT: PyErr_SetString - does not affect reference counts
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
ConfigBackendPoolDHCPv4_getAllClientClasses4(ConfigBackendPoolDHCPv4Object *self, PyObject *args) {
    const char *backend;
    PyObject *selector;

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
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
        ClientClassDictionary classes = self->ptr->getAllClientClasses4(backend_selector, server_selector);
        const ClientClassDefListPtr& class_list = classes.getClasses();
        // REFCOUNT: PyList_New - returns new reference
        PyObject *list = PyList_New(0);
        if (!list) {
            return (0);
        }
        
        for (const auto& class_def : *class_list) {
            // REFCOUNT: ClientClassDef_from_ptr - returns new reference
            PyObject *obj = ClientClassDef_from_ptr(const_cast<ClientClassDefPtr&>(class_def));
            if (!obj) {
                // REFCOUNT: Py_DECREF - decrements reference count
                Py_DECREF(list);
                return (0);
            }
            // REFCOUNT: PyList_Append - does not steal reference
            if (PyList_Append(list, obj) < 0) {
                // REFCOUNT: Py_DECREF - decrements reference count
                Py_DECREF(obj);
                Py_DECREF(list);
                return (0);
            }
            // REFCOUNT: Py_DECREF - decrements reference count (PyList_Append doesn't steal)
            Py_DECREF(obj);
        }
        
        return list;
    }
    catch (const exception &e) {
        // REFCOUNT: PyErr_SetString - does not affect reference counts
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
        // REFCOUNT: PyList_New - returns new reference
        PyObject *list = PyList_New(servers.size());
        if (!list) {
            return (0);
        }
        try {
            int pos = 0;
            for (auto it = servers.begin(); it != servers.cend(); ++it, ++pos) {
                ServerPtr server = *it;
                // REFCOUNT: Server_from_ptr - returns new reference
                PyObject *elem = Server_from_ptr(server);
                // REFCOUNT: PyList_SetItem - steals reference
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

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
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
            // REFCOUNT: Subnet4_from_ptr - returns new reference
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

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
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
        // REFCOUNT: PyList_New - returns new reference
        PyObject *list = PyList_New(subnets.size());
        if (!list) {
            return (0);
        }
        try {
            int pos = 0;
            for (auto it = subnets.begin(); it != subnets.cend(); ++it, ++pos) {
                Subnet4Ptr subnet = *it;
                // REFCOUNT: Subnet4_from_ptr - returns new reference
                PyObject *elem = Subnet4_from_ptr(subnet);
                // REFCOUNT: PyList_SetItem - steals reference
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
    // REFCOUNT: PyObject_New - returns new reference
    ConfigBackendPoolDHCPv4Object *self = PyObject_New(ConfigBackendPoolDHCPv4Object, &ConfigBackendPoolDHCPv4Type);
    if (self) {
        new(&self->ptr) ConfigBackendPoolDHCPv4Ptr;
        self->ptr = ptr;
    }
    return ((PyObject *)self);
}

static PyMethodDef ConfigBackendPoolDHCPv4_methods[] = {
    {"createUpdateClientClass4", (PyCFunction) ConfigBackendPoolDHCPv4_createUpdateClientClass4, METH_VARARGS,
     "Creates or updates a client class."},
    {"createUpdateServer4", (PyCFunction) ConfigBackendPoolDHCPv4_createUpdateServer4, METH_VARARGS,
     "Creates or updates a server."},
    {"createUpdateSubnet4", (PyCFunction) ConfigBackendPoolDHCPv4_createUpdateSubnet4, METH_VARARGS,
     "Creates or updates a subnet."},
    {"deleteClientClass4", (PyCFunction) ConfigBackendPoolDHCPv4_deleteClientClass4, METH_VARARGS,
     "Deletes a client class by name."},
    {"deleteServer4", (PyCFunction) ConfigBackendPoolDHCPv4_deleteServer4, METH_VARARGS,
     "Deletes a server from the backend."},
    {"deleteSubnet4", (PyCFunction) ConfigBackendPoolDHCPv4_deleteSubnet4, METH_VARARGS,
     "Deletes subnet by identifier."},
    {"getAllClientClasses4", (PyCFunction) ConfigBackendPoolDHCPv4_getAllClientClasses4, METH_VARARGS,
     "Retrieves all client classes."},
    {"getAllServers4", (PyCFunction) ConfigBackendPoolDHCPv4_getAllServers4, METH_VARARGS,
     "Retrieves all servers from the particular backend."},
    {"getAllSubnets4", (PyCFunction) ConfigBackendPoolDHCPv4_getAllSubnets4, METH_VARARGS,
     "Collection of subnets or empty collection if no subnet found."},
    {"getClientClass4", (PyCFunction) ConfigBackendPoolDHCPv4_getClientClass4, METH_VARARGS,
     "Retrieves a client class by name."},
    {"getSubnet4", (PyCFunction) ConfigBackendPoolDHCPv4_getSubnet4, METH_VARARGS,
     "Retrieves a single subnet by subnet identifier."},
    {0}  // Sentinel
};

// tp_dealloc - called when refcount is zero
static void
ConfigBackendPoolDHCPv4_dealloc(ConfigBackendPoolDHCPv4Object *self) {
    self->ptr.~ConfigBackendPoolDHCPv4Ptr();
    Py_TYPE(self)->tp_free((PyObject *) self);
}


// tp_init - called after tp_new has returned an instance
static int
ConfigBackendPoolDHCPv4_init(ConfigBackendPoolDHCPv4Object *self, PyObject *args, PyObject *kwds) {
    PyErr_SetString(PyExc_TypeError, "cannot create 'kea.ConfigBackendPoolDHCPv4' instances");
    return (-1);
}

PyTypeObject ConfigBackendPoolDHCPv4Type = {
    .ob_base = PyObject_HEAD_INIT(0)
    .tp_name = "kea.ConfigBackendPoolDHCPv4",
    .tp_basicsize = sizeof(ConfigBackendPoolDHCPv4Object),
    .tp_dealloc = (destructor) ConfigBackendPoolDHCPv4_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Kea server ConfigBackendPoolDHCPv4"),
    .tp_methods = ConfigBackendPoolDHCPv4_methods,
    .tp_init = (initproc) ConfigBackendPoolDHCPv4_init,
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = PyType_GenericNew,
};

int
ConfigBackendPoolDHCPv4_define() {
    // PyType_Ready - finish type initialisation
    if (PyType_Ready(&ConfigBackendPoolDHCPv4Type) < 0) {
        return (1);
    }
    Py_INCREF(&ConfigBackendPoolDHCPv4Type);
    // REFCOUNT: PyModule_AddObject steals reference on success
    if (PyModule_AddObject(kea_module, "ConfigBackendPoolDHCPv4", (PyObject *) &ConfigBackendPoolDHCPv4Type) < 0) {
        Py_DECREF(&ConfigBackendPoolDHCPv4Type);
        return (1);
    }

    return (0);
}

}
