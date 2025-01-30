#include "keamodule.h"

using namespace std;
using namespace isc::hooks;

extern "C" {

static const char *hooks[] = {
    "dhcp4_srv_configured",
    "cb4_updated",
    "buffer4_receive",
    "pkt4_receive",
    "subnet4_select",
    "host4_identifier",
    "lease4_select",
    "lease4_renew",
    "lease4_release",
    "lease4_decline",
    "leases4_committed",
    "pkt4_send",
    "buffer4_send",
    "lease4_expire",
    "lease4_recover",
    "command_processed"
};
#define num_hooks (sizeof(hooks) / sizeof(hooks[0]))

// dict of CalloutClosureObject objects by name
static PyObject *callout_closures;

int
Callouts_add_closure(CalloutClosureObject *obj) {
    // REFCOUNT: PyDict_SetItem - reference neutral
    return (PyDict_SetItem(callout_closures, obj->name, (PyObject *)obj));
}

static int
register_callouts(LibraryHandle *handle) {
    for (unsigned int i = 0; i < num_hooks; i++) {
        PyObject *callout;

        // REFCOUNT: PyObject_GetAttrString - returns new reference
        callout = PyObject_GetAttrString(hook_module, hooks[i]);
        if (!callout) {
            PyErr_Clear();
            continue;
        }
        if (!PyCallable_Check(callout)) {
            log_error(string(hooks[i]) + " must be callable");
            Py_DECREF(callout);
            return (1);
        }
        // REFCOUNT: PyUnicode_FromString - returns new reference
        PyObject *name = PyUnicode_FromString(hooks[i]);
        if (!name) {
            Py_DECREF(callout);
            return (1);
        }
        // REFCOUNT: CalloutClosure_from_object - returns new reference
        CalloutClosureObject *obj = (CalloutClosureObject *) CalloutClosure_from_object(name, callout);
        if (!obj) {
            Py_DECREF(callout);
            Py_DECREF(name);
            return (1);
        }
        Py_DECREF(callout);
        Py_DECREF(name);
        // REFCOUNT: Callouts_add_closure - manage obj in dict
        if (Callouts_add_closure(obj)) {
            Py_DECREF(obj);
            return (1);
        }
        Py_DECREF(obj);
        try {
            handle->registerCallout(hooks[i], (CalloutPtr)obj->bound_callout);
        }
        catch (const exception &e) {
            PyErr_SetString(PyExc_TypeError, e.what());
            return (0);
        }
    }
    return (0);
}

static int
unregister_callouts() {
    Py_XDECREF(callout_closures);
    callout_closures = 0;
    return (0);
}

static int
call_load_callout(LibraryHandle *handle) {
    PyObject *load;
    PyObject *py_handle = 0;
    PyObject *py_result = 0;
    int res = 1;

    // REFCOUNT: PyObject_GetAttrString - returns new reference
    load = PyObject_GetAttrString(hook_module, "load");
    if (!load) {
        PyErr_Clear();
        return (0);
    }
    if (!PyCallable_Check(load)) {
        log_error("load must be callable");
        goto error;
    }
    // REFCOUNT: LibraryHandle_from_handle - returns new reference
    py_handle = LibraryHandle_from_handle(handle);
    if (!py_handle) {
        log_error("could not create LibraryHandle");
        goto error;
    }
    // REFCOUNT: PyObject_CallFunction - returns new reference
    py_result = PyObject_CallFunction(load, "O", py_handle);
    if (!py_result) {
        log_python_traceback();
        goto error;
    }
    if (!PyLong_CheckExact(py_result)) {
        log_error("load must return integer");
        goto error;
    }

    res = PyLong_AsLong(py_result);

error:
    Py_XDECREF(py_result);
    Py_XDECREF(py_handle);
    Py_XDECREF(load);
    return (res);
}

int
Callouts_register(LibraryHandle *handle) {
    int res;

    // REFCOUNT: PyDict_New - returns new reference
    callout_closures = PyDict_New();
    if (!callout_closures) {
        return (1);
    }
    res = call_load_callout(handle);
    if (!res) {
        res = register_callouts(handle);
    }

    return (res);
}

int
Callouts_unregister() {
    return (unregister_callouts());
}

}
