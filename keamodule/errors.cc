#include "keamodule.h"

using namespace std;

extern "C" {
static PyObject *traceback_module;

int
Errors_initialize() {
    // REFCOUNT: PyImport_ImportModule - returns new reference
    traceback_module = PyImport_ImportModule("traceback");
    if (!traceback_module) {
        log_error("import traceback failed");
        return (1);
    }

    return (0);
}

int
Errors_finalize() {
    if (traceback_module) {
        Py_DECREF(traceback_module);
        traceback_module = 0;
    }

    return (0);
}

static string
to_string(PyObject *object) {
    if (!object) {
        return ("NULL");
    }
    // REFCOUNT: PyObject_Str - returns new reference
    PyObject *str = PyObject_Str(object);
    if (!str) {
        return ("NULL");
    }
    // REFCOUNT: PyUnicode_AsUTF8 - returns UTF-8 encoding of str - buffer cached in str
    string ret(PyUnicode_AsUTF8(str));
    Py_DECREF(str);

    return (ret);
}

int
format_python_traceback(PyObject *exc_type, PyObject *exc_value, PyObject *exc_traceback, string &traceback) {
    // import traceback
    // exc_type, exc_value, exc_traceback = sys.exc_info()
    // return ''.join(traceback.format_exception(exc_type, exc_value, exc_traceback))
    PyObject *format_exception = 0;
    PyObject *line_list = 0;
    PyObject *empty_string = 0;
    PyObject *formatted_error = 0;
    int res = 1;

    // REFCOUNT: PyErr_NormalizeException - reference neutral
    PyErr_NormalizeException(&exc_type, &exc_value, &exc_traceback);

    if (!exc_traceback || !traceback_module) {
        // PyExceptionClass_Name - returns C name of exception
        traceback = string(PyExceptionClass_Name(exc_type)) + ": " + to_string(exc_value);
        res = 0;
        goto error;
    }
    else {
        // PyObject_GetAttrString - returns new reference
        format_exception = PyObject_GetAttrString(traceback_module, "format_exception");
        if (!format_exception) {
            log_error("traceback.format_exception not found");
            goto error;
        }
        // REFCOUNT: PyObject_CallFunction - returns new reference
        line_list = PyObject_CallFunction(format_exception, "OOO", exc_type, exc_value, exc_traceback);
        if (!line_list) {
            log_error("traceback.format_exception(" + to_string(exc_type) + ", " + to_string(exc_value) + ", " + to_string(exc_traceback) + ") failed");
            goto error;
        }
    }
    // REFCOUNT: PyUnicode_FromString - returns new reference
    empty_string = PyUnicode_FromString("");
    if (!empty_string) {
        goto error;
    }
    // REFCOUNT: PyUnicode_Join - returns new reference
    formatted_error = PyUnicode_Join(empty_string, line_list);
    if (!formatted_error) {
        log_error("''.join(line_list) failed");
        goto error;
    }
    // REFCOUNT: PyUnicode_AsUTF8 - returns UTF-8 encoding of str - buffer cached in str
    traceback = string(PyUnicode_AsUTF8(formatted_error));
    res = 0;

error:
    PyErr_Clear();

    Py_XDECREF(formatted_error);
    Py_XDECREF(empty_string);
    Py_XDECREF(line_list);
    Py_XDECREF(format_exception);
    Py_XDECREF(exc_type);
    Py_XDECREF(exc_value);
    Py_XDECREF(exc_traceback);

    return (res);
}

int
log_python_traceback() {
    PyObject *exc_type, *exc_value, *exc_traceback;
    string traceback;

    // REFCOUNT: PyErr_Fetch - creates new references
    PyErr_Fetch(&exc_type, &exc_value, &exc_traceback);
    // REFCOUNT: steals reference to exc_* objects
    if (!format_python_traceback(exc_type, exc_value, exc_traceback, traceback)) {
        log_error(traceback);
        return (0);
    }

    return (1);
}

}
