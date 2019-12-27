#include "keamodule.h"

using namespace isc::hooks;

extern "C" {

int
Callouts_define(LibraryHandle *handle, PyObject *hook_module) {
    // look for load
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
        LOG_ERROR(*kea_logger, *kea_message_id).arg("load must be callable");
        goto error;
    }
    py_handle = LibraryHandle_from_handle(handle);
    if (!py_handle) {
        LOG_ERROR(*kea_logger, *kea_message_id).arg("could not create LibraryHandle");
        goto error;
    }
    py_result = PyObject_CallFunction(load, "O", py_handle);
    if (!py_result) {
        log_python_traceback();
        goto error;
    }
    if (!PyLong_CheckExact(py_result)) {
        LOG_ERROR(*kea_logger, *kea_message_id).arg("load must return integer");
        goto error;
    }

    res = PyLong_AsLong(py_result);

error:
    Py_XDECREF(py_result);
    Py_XDECREF(py_handle);
    Py_XDECREF(load);
    return (res);
}

}
