#include <hooks/hooks.h>

using namespace isc::hooks;

extern "C" {
#include "keamodule.h"
#define KEA_MODULE
#include "keacapsule.h"

static PyObject *hook_module;
static PyObject *traceback_module;
static LogFunctions *log_callbacks;

typedef struct {
    PyObject_HEAD
} LoggerObject;

static PyObject *
Logger_debug(PyObject *self, PyObject *args) {
    char *msg;

    if (!PyArg_ParseTuple(args, "s", &msg)) {
        return NULL;
    }
    log_callbacks->debug(msg);

    Py_RETURN_NONE;
}

static PyObject *
Logger_info(PyObject *self, PyObject *args) {
    char *msg;

    if (!PyArg_ParseTuple(args, "s", &msg)) {
        return NULL;
    }
    log_callbacks->info(msg);

    Py_RETURN_NONE;
}

static PyObject *
Logger_warn(PyObject *self, PyObject *args) {
    char *msg;

    if (!PyArg_ParseTuple(args, "s", &msg)) {
        return NULL;
    }
    log_callbacks->warn(msg);

    Py_RETURN_NONE;
}


static PyObject *
Logger_error(PyObject *self, PyObject *args) {
    char *msg;

    if (!PyArg_ParseTuple(args, "s", &msg)) {
        return NULL;
    }
    log_callbacks->error(msg);

    Py_RETURN_NONE;
}

static PyObject *
Logger_fatal(PyObject *self, PyObject *args) {
    char *msg;

    if (!PyArg_ParseTuple(args, "s", &msg)) {
        return NULL;
    }
    log_callbacks->fatal(msg);

    Py_RETURN_NONE;
}

static PyMethodDef Logger_methods[] = {
    {"debug", (PyCFunction) Logger_debug, METH_VARARGS,
     "Log a debug message to the kea logger"},
    {"info", (PyCFunction)(void(*)(void))Logger_info, METH_VARARGS,
     "Log an info message to the kea logger."},
    {"warn", (PyCFunction)(void(*)(void))Logger_warn, METH_VARARGS,
     "Log an warn message to the kea logger."},
    {"error", (PyCFunction)(void(*)(void))Logger_error, METH_VARARGS,
     "Log an error message to the kea logger."},
    {"fatal", (PyCFunction)(void(*)(void))Logger_fatal, METH_VARARGS,
     "Log a fatal message to the kea logger."},
    {NULL}  /* Sentinel */
};

static void
Logger_dealloc(LoggerObject *self) {
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyTypeObject LoggerType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "kea.Logger",                               /* tp_name */
    sizeof(LoggerObject),                       /* tp_basicsize */
    0,                                          /* tp_itemsize */
    (destructor)Logger_dealloc,                 /* tp_dealloc */
    0,                                          /* tp_vectorcall_offset */
    0,                                          /* tp_getattr */
    0,                                          /* tp_setattr */
    0,                                          /* tp_as_async */
    (reprfunc)0,                                /* tp_repr */
    0,                                          /* tp_as_number */
    0,                                          /* tp_as_sequence */
    0,                                          /* tp_as_mapping */
    0,                                          /* tp_hash */
    0,                                          /* tp_call */
    0,                                          /* tp_str */
    0,                                          /* tp_getattro */
    0,                                          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                         /* tp_flags */
    "Kea server logging",                       /* tp_doc */
    0,                                          /* tp_traverse */
    0,                                          /* tp_clear */
    0,                                          /* tp_richcompare */
    0,                                          /* tp_weaklistoffset */
    0,                                          /* tp_iter */
    0,                                          /* tp_iternext */
    Logger_methods,                             /* tp_methods */
    0,                                          /* tp_members */
    0,                                          /* tp_getset */
    0,                                          /* tp_base */
    0,                                          /* tp_dict */
    0,                                          /* tp_descr_get */
    0,                                          /* tp_descr_set */
    0,                                          /* tp_dictoffset */
    0,                                          /* tp_init */
    PyType_GenericAlloc,                        /* tp_alloc */
    PyType_GenericNew                           /* tp_new */
};

static PyObject *
import_python_module(const char *module_name) {
    PyObject *name = 0;

    name = PyUnicode_DecodeFSDefault(module_name);
    if (!name) {
        return NULL;
    }
    PyObject *module = PyImport_Import(name);
    Py_DECREF(name);

    return (module);
}

static int
format_python_traceback() {
    // import traceback
    // exc_type, exc_value, exc_traceback = sys.exc_info()
    // return ''.join(traceback.format_exception(exc_type, exc_value, exc_traceback))
    PyObject *exc_type, *exc_value, *exc_traceback;
    PyObject *format_exception = NULL;
    PyObject *line_list = NULL;
    PyObject *empty_string = NULL;
    PyObject *formatted_error = NULL;
    int res = 1;

    PyErr_Fetch(&exc_type, &exc_value, &exc_traceback);

    if (!traceback_module) {
        traceback_module = import_python_module("traceback");
        if (!traceback_module) {
            goto error;
        }
    }

    format_exception = PyObject_GetAttrString(traceback_module, "format_exception");
    if (!format_exception) {
        goto error;
    }
    line_list = PyObject_CallFunction(format_exception, "OOO", exc_type, exc_value, exc_traceback);
    if (!line_list) {
        goto error;
    }
    empty_string = PyUnicode_FromString("");
    if (!empty_string) {
        goto error;
    }
    formatted_error = PyUnicode_Join(empty_string, line_list);
    if (!formatted_error) {
        goto error;
    }
    log_callbacks->error(PyUnicode_AsUTF8(formatted_error));

    res = 0;

error:
    Py_XDECREF(formatted_error);
    Py_XDECREF(empty_string);
    Py_XDECREF(line_list);
    Py_XDECREF(format_exception);
    Py_XDECREF(exc_type);
    Py_XDECREF(exc_value);
    Py_XDECREF(exc_traceback);

    return (res);
}

static int
Kea_Bootstrap(LibraryHandle *handle, LogFunctions *log_functions, const char *module_name) {
    if (PyType_Ready(&LoggerType) < 0) {
        return (1);
    }
    Py_INCREF(&LoggerType);
    LoggerObject *logger = PyObject_New(LoggerObject, &LoggerType);
    if (!logger) {
        Py_DECREF(&LoggerType);
        return (1);
    }
    log_callbacks = log_functions;
    // PyModule_AddObject steals reference on success
    if (PyModule_AddObject(kea_module, "logger", (PyObject*)logger) < 0) {
        Py_DECREF(&LoggerType);
        Py_DECREF(logger);
        return (1);
    }
    hook_module = import_python_module(module_name);
    if (hook_module == NULL) {
        format_python_traceback();
        return (1);
    }

    return (0);
}

static int
Kea_Shutdown() {
    Py_INCREF(Py_None);
    if (PyModule_AddObject(kea_module, "logger", Py_None) < 0) {
        Py_DECREF(Py_None);
    }
    Py_DECREF(&LoggerType);
    if (hook_module) {
        Py_DECREF(hook_module);
        hook_module = NULL;
    }

    return (0);
}

int
Logger_define_capsule(PyObject *module) {
    static void *kea_capsule[Kea_API_pointers];
    PyObject *c_api_object = 0;

    // initialize the C API pointer array
    kea_capsule[Kea_Bootstrap_NUM] = (void *)Kea_Bootstrap;
    kea_capsule[Kea_Shutdown_NUM] = (void *)Kea_Shutdown;
    // create a Capsule containing the API pointer array's address
    c_api_object = PyCapsule_New((void *)kea_capsule, "kea._C_API", NULL);
    if (PyModule_AddObject(module, "_C_API", c_api_object) < 0) {
        Py_XDECREF(c_api_object);
        Py_DECREF(module);
        return (1);
    }
    // initialize logger of None - will be replaced with Capsule Kea_Bootstrap
    Py_INCREF(Py_None);
    if (PyModule_AddObject(module, "logger", Py_None) < 0) {
        Py_DECREF(Py_None);
        Py_DECREF(module);
        return (1);
    }

    return (0);
}

}
