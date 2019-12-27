#include "keamodule.h"

using namespace std;
using namespace isc::hooks;

extern "C" {

static int
call_callout(const char *name, CalloutHandle &handle) {
    PyObject *callout;
    PyObject *py_handle = 0;
    PyObject *py_result = 0;
    int res = 1;

    callout = PyObject_GetAttrString(hook_module, name);
    if (!callout) {
        PyErr_Clear();
        return (0);
    }
    if (!PyCallable_Check(callout)) {
        log_error("load must be callable");
        goto error;
    }
    py_handle = CalloutHandle_from_handle(&handle);
    if (!py_handle) {
        log_error("could not create CalloutHandle");
        goto error;
    }
    py_result = PyObject_CallFunction(callout, "O", py_handle);
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
    Py_XDECREF(callout);
    return (res);
}

#define make_proxy_func(name) \
static int \
proxy_ ##name(CalloutHandle &handle) { \
    return (call_callout(#name, handle)); \
}

make_proxy_func(dhcp4_srv_configured)
make_proxy_func(cb4_updated)
make_proxy_func(buffer4_receive)
make_proxy_func(pkt4_receive)
make_proxy_func(subnet4_select)
make_proxy_func(host4_identifier)
make_proxy_func(lease4_select)
make_proxy_func(lease4_renew)
make_proxy_func(lease4_release)
make_proxy_func(lease4_decline)
make_proxy_func(leases4_committed)
make_proxy_func(pkt4_send)
make_proxy_func(buffer4_send)
make_proxy_func(lease4_expire)
make_proxy_func(lease4_recover)
make_proxy_func(command_processed)

typedef struct {
    const char *name;
    int (*proxy_func)(CalloutHandle &handle);
    bool is_registered;
} CalloutProxy;
#define callout_proxy(name) {#name, proxy_ ##name }

static CalloutProxy callout_proxies[] = {
    callout_proxy(dhcp4_srv_configured),
    callout_proxy(cb4_updated),
    callout_proxy(buffer4_receive),
    callout_proxy(pkt4_receive),
    callout_proxy(subnet4_select),
    callout_proxy(host4_identifier),
    callout_proxy(lease4_select),
    callout_proxy(lease4_renew),
    callout_proxy(lease4_release),
    callout_proxy(lease4_decline),
    callout_proxy(leases4_committed),
    callout_proxy(pkt4_send),
    callout_proxy(buffer4_send),
    callout_proxy(lease4_expire),
    callout_proxy(lease4_recover),
    callout_proxy(command_processed)
};
#define num_callout_proxies (sizeof(callout_proxies) / sizeof(callout_proxies[0]))

static int
register_callouts(LibraryHandle *handle) {
    for (unsigned int i = 0; i < num_callout_proxies; i++) {
        CalloutProxy *proxy = &callout_proxies[i];
        PyObject *python_func;

        python_func = PyObject_GetAttrString(hook_module, proxy->name);
        if (!python_func) {
            PyErr_Clear();
            proxy->is_registered = false;
            continue;
        }
        if (!PyCallable_Check(python_func)) {
            log_error(string(proxy->name) + " must be callable");
            Py_DECREF(python_func);
            proxy->is_registered = false;
            return (1);
        }
        Py_DECREF(python_func);
        handle->registerCallout(proxy->name, proxy->proxy_func);
        proxy->is_registered = true;
    }
    return (0);
}

// TODO add unregister_callouts

static int
call_load_callout(LibraryHandle *handle) {
    PyObject *load;
    PyObject *py_handle = 0;
    PyObject *py_result = 0;
    int res = 1;

    load = PyObject_GetAttrString(hook_module, "load");
    if (!load) {
        PyErr_Clear();
        return (0);
    }
    if (!PyCallable_Check(load)) {
        log_error("load must be callable");
        goto error;
    }
    py_handle = LibraryHandle_from_handle(handle);
    if (!py_handle) {
        log_error("could not create LibraryHandle");
        goto error;
    }
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
Callouts_define(LibraryHandle *handle) {
    int res;

    res = call_load_callout(handle);
    if (!res) {
        res = register_callouts(handle);
    }

    return (res);
}

}
