#include "keamodule.h"
#include <dhcpsrv/lease_mgr_factory.h>

using namespace std;
using namespace isc::dhcp;
using namespace isc::asiolink;

extern "C" {

static PyObject *
lease_list_from_collection(Lease4Collection &leases) {
    // REFCOUNT: PyList_New - returns new reference
    PyObject *list = PyList_New(leases.size());
    if (!list) {
        return (0);
    }
    try {
        int pos = 0;
        for (auto lease : leases) {
            // REFCOUNT: Lease4_from_ptr - returns new reference
            PyObject *obj = Lease4_from_ptr(lease);
            if (obj == 0) {
                Py_DECREF(list);
                return (0);
            }
            // REFCOUNT: PyList_SetItem - steals reference
            if (PyList_SetItem(list, pos++, obj) < 0) {
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

#if HAVE_GETLEASE4_METHOD
static PyObject *
LeaseMgr_getLease4(LeaseMgrObject *self, PyObject *args, PyObject *kwargs) {
    static const char *kwlist[] = {"addr", "hwaddr", "client_id", "subnet_id", NULL};
    char *addr = 0;
    char *hwaddr = 0;
    char *client_id = 0;
    unsigned long subnet_id = 0;
    bool have_subnet_id = false;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|sssk", (char **)kwlist, &addr, &hwaddr, &client_id, &subnet_id)) {
        return (0);
    }
    if (kwargs != 0) {
        // REFCOUNT: PyDict_GetItemString - returns borrowed reference
        PyObject *tmp = PyDict_GetItemString(kwargs, "subnet_id");
        if (tmp) {
            have_subnet_id = true;
        }
    }

    try {
        Lease4Ptr ptr;
        if (addr != 0 && !hwaddr && !client_id && !have_subnet_id) {
            // Lease4Ptr getLease4(const IOAddress &addr)
            ptr = self->mgr->getLease4(IOAddress(string(addr)));
        }
        else if (!addr && hwaddr != 0 && !client_id && have_subnet_id) {
            // Lease4Ptr getLease4(const HWAddr &hwaddr, SubnetID subnet_id)
            HWAddr hw = HWAddr::fromText(hwaddr);
            ptr = self->mgr->getLease4(hw, subnet_id);
        }
        else if (!addr && !hwaddr && client_id != 0 && have_subnet_id) {
            // Lease4Ptr getLease4(const ClientId &clientid, SubnetID subnet_id)
            ClientIdPtr clientid_ptr = ClientId::fromText(client_id);
            ptr = self->mgr->getLease4(*clientid_ptr, subnet_id);
        }
        else if (!addr && hwaddr != 0 && client_id != 0 && have_subnet_id) {
            // Lease4Ptr getLease4(const ClientId &client_id, const HWAddr &hwaddr, SubnetID subnet_id)
            ClientIdPtr clientid_ptr = ClientId::fromText(client_id);
            HWAddr hw = HWAddr::fromText(hwaddr);
            ptr = self->mgr->getLease4(*clientid_ptr, hw, subnet_id);
        }
        else if (!addr && hwaddr != 0 && !client_id && !have_subnet_id) {
            // Lease4Collection getLease4(const HWAddr &hwaddr)
            HWAddr hw = HWAddr::fromText(hwaddr);
            Lease4Collection leases = self->mgr->getLease4(hw);
            // REFCOUNT: lease_list_from_collection - returns new reference
            return (lease_list_from_collection(leases));
        }
        else if (!addr && !hwaddr && client_id != 0 && !have_subnet_id) {
            // Lease4Collection getLease4(const ClientId &clientid)
            ClientIdPtr clientid_ptr = ClientId::fromText(client_id);
            Lease4Collection leases = self->mgr->getLease4(*clientid_ptr);
            // REFCOUNT: lease_list_from_collection - returns new reference
            return (lease_list_from_collection(leases));
        }
        else {
            PyErr_SetString(PyExc_TypeError, "invalid argument combination");
            return (0);
        }
        if (!ptr) {
            Py_RETURN_NONE;
        }
        // REFCOUNT: Lease4_from_ptr - returns new reference
        return (Lease4_from_ptr(ptr));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}
#endif

static PyObject *
LeaseMgr_getLeases4(LeaseMgrObject *self, PyObject *args, PyObject *kwargs) {
#if MISSING_GETLEASES4_HOSTNAME
    #define KWLIST {"subnet_id", "lower_bound_address", "page_size", NULL}
    #define KWFORMAT "|ksk"
    #define KWVARS &subnet_id, &lower_bound_address, &page_size
#else
    #define KWLIST {"subnet_id", "hostname", "lower_bound_address", "page_size", NULL}
    #define KWFORMAT "|kssk"
    #define KWVARS &subnet_id, &hostname, &lower_bound_address, &page_size
#endif
    static const char *kwlist[] = KWLIST;
    unsigned long subnet_id = 0;
    char *hostname = 0;
    char *lower_bound_address = 0;
    unsigned long page_size = 0;
    bool have_subnet_id = false;
    bool have_page_size = false;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, KWFORMAT, (char **)kwlist, KWVARS)) {
        return (0);
    }
    if (kwargs != 0) {
        // REFCOUNT: PyDict_GetItemString - returns borrowed reference
        PyObject *tmp = PyDict_GetItemString(kwargs, "subnet_id");
        if (tmp) {
            have_subnet_id = true;
        }
        // REFCOUNT: PyDict_GetItemString - returns borrowed reference
        tmp = PyDict_GetItemString(kwargs, "page_size");
        if (tmp) {
            have_page_size = true;
        }
    }

    try {
        Lease4Collection leases;
        if (have_subnet_id && !hostname && !lower_bound_address && !have_page_size) {
            // Lease4Collection getLeases4(SubnetID subnet_id)
            leases = self->mgr->getLeases4((SubnetID) subnet_id);
        }
#ifndef MISSING_GETLEASES4_HOSTNAME
        else if (!have_subnet_id && hostname != 0 && !lower_bound_address && !have_page_size) {
            // Lease4Collection getLeases4(const std::string &hostname)
            leases = self->mgr->getLeases4(string(hostname));
        }
#endif
        else if (!have_subnet_id && !hostname && !lower_bound_address && !have_page_size) {
            // Lease4Collection getLeases4()
            leases = self->mgr->getLeases4();
        }
        else if (!have_subnet_id && !hostname && lower_bound_address != 0 && have_page_size) {
            // Lease4Collection getLeases4(const IOAddress &lower_bound_address, const LeasePageSize &page_size)
            leases = self->mgr->getLeases4(IOAddress(string(lower_bound_address)), LeasePageSize(page_size));
        }
        else {
            PyErr_SetString(PyExc_TypeError, "invalid argument combination");
            return (0);
        }

        // REFCOUNT: lease_list_from_collection - returns new reference
        return (lease_list_from_collection(leases));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
LeaseMgr_addLease(LeaseMgrObject *self, PyObject *args) {
    Lease4Object *lease;

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
    if (!PyArg_ParseTuple(args, "O!", &Lease4Type, &lease)) {
        return (0);
    }

    try {
        self->mgr->addLease(lease->ptr);
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
LeaseMgr_deleteLease(LeaseMgrObject *self, PyObject *args) {
#if HAVE_DELETELEASE_ADDR
    char *addr;

    if (!PyArg_ParseTuple(args, "s", &addr)) {
        return (0);
    }
#else
    Lease4Object *lease;

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
    if (!PyArg_ParseTuple(args, "O!", &Lease4Type, &lease)) {
        return (0);
    }
#endif

    try {
#if HAVE_DELETELEASE_ADDR
        bool result = self->mgr->deleteLease(IOAddress(string(addr)));
#else
        bool result = self->mgr->deleteLease(lease->ptr);
#endif
        if (result) {
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
LeaseMgr_updateLease4(LeaseMgrObject *self, PyObject *args) {
    Lease4Object *lease;

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
    if (!PyArg_ParseTuple(args, "O!", &Lease4Type, &lease)) {
        return (0);
    }

    try {
        self->mgr->updateLease4(lease->ptr);
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
LeaseMgr_wipeLeases4(LeaseMgrObject *self, PyObject *args) {
    unsigned long subnet_id;

    if (!PyArg_ParseTuple(args, "k", &subnet_id)) {
        return (0);
    }

    try {
        size_t result = self->mgr->wipeLeases4((SubnetID) subnet_id);
        // REFCOUNT: PyLong_FromLong - returns new references
        return (PyLong_FromLong(result));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyMethodDef LeaseMgr_methods[] = {
#if HAVE_GETLEASE4_METHOD
    {"getLease4", (PyCFunction) LeaseMgr_getLease4, METH_VARARGS | METH_KEYWORDS,
     "Returns an IPv4 lease for specified IPv4 address."},
#endif
    {"getLeases4", (PyCFunction) LeaseMgr_getLeases4, METH_VARARGS | METH_KEYWORDS,
     "Returns all IPv4 leases for the particular subnet identifier."},
    {"addLease", (PyCFunction) LeaseMgr_addLease, METH_VARARGS,
     "Adds an IPv4 lease."},
    {"deleteLease", (PyCFunction) LeaseMgr_deleteLease, METH_VARARGS,
     "Deletes a lease."},
    {"updateLease4", (PyCFunction) LeaseMgr_updateLease4, METH_VARARGS,
     "Updates IPv4 lease."},
    {"wipeLeases4", (PyCFunction) LeaseMgr_wipeLeases4, METH_VARARGS,
     "Virtual method which removes specified leases."},
    {0}  // Sentinel
};

// tp_dealloc - called when refcount is zero
static void
LeaseMgr_dealloc(LeaseMgrObject *self) {
    Py_TYPE(self)->tp_free((PyObject *) self);
}

// tp_init - called after tp_new has returned an instance
static int
LeaseMgr_init(LeaseMgrObject *self, PyObject *args, PyObject *kwds) {
    if (kwds != 0) {
        PyErr_SetString(PyExc_TypeError, "keyword arguments are not supported");
        return (-1);
    }
    if (!PyArg_ParseTuple(args, "")) {
        return (-1);
    }

    try {
        self->mgr = &LeaseMgrFactory::instance();
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (-1);
    }

    return (0);
}

// tp_new - allocate space and initialisation that can be repeated
static PyObject *
LeaseMgr_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    LeaseMgrObject *self;
    self = (LeaseMgrObject *) type->tp_alloc(type, 0);
    if (self) {
        self->mgr = 0;
    }
    return ((PyObject *) self);
}

PyTypeObject LeaseMgrType = {
    .ob_base = PyObject_HEAD_INIT(0)
    .tp_name = "kea.LeaseMgr",
    .tp_basicsize = sizeof(LeaseMgrObject),
    .tp_dealloc = (destructor) LeaseMgr_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Kea server LeaseMgr"),
    .tp_methods = LeaseMgr_methods,
    .tp_init = (initproc) LeaseMgr_init,
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = LeaseMgr_new,
};

int
LeaseMgr_define() {
    // PyType_Ready - finish type initialisation
    if (PyType_Ready(&LeaseMgrType) < 0) {
        return (1);
    }
    Py_INCREF(&LeaseMgrType);
    // REFCOUNT: PyModule_AddObject steals reference on success
    if (PyModule_AddObject(kea_module, "LeaseMgr", (PyObject *) &LeaseMgrType) < 0) {
        Py_DECREF(&LeaseMgrType);
        return (1);
    }

    return (0);
}

}
