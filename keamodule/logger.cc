#include <hooks/hooks.h>

using namespace isc::hooks;

extern "C" {
#include "keamodule.h"
#define KEA_MODULE
#include "keacapsule.h"

PyObject *hook_module;

typedef struct {
    PyObject_HEAD

    LogFunctions *log_functions;
} LoggerObject;

static PyObject *
Logger_debug(PyObject *self, PyObject *args) {
    char *msg;

    if (!PyArg_ParseTuple(args, "s", &msg)) {
        return NULL;
    }
    ((LoggerObject*) self)->log_functions->debug(msg);

    Py_RETURN_NONE;
}

static PyObject *
Logger_info(PyObject *self, PyObject *args) {
    char *msg;

    if (!PyArg_ParseTuple(args, "s", &msg)) {
        return NULL;
    }
    ((LoggerObject*) self)->log_functions->info(msg);

    Py_RETURN_NONE;
}

static PyObject *
Logger_warn(PyObject *self, PyObject *args) {
    char *msg;

    if (!PyArg_ParseTuple(args, "s", &msg)) {
        return NULL;
    }
    ((LoggerObject*) self)->log_functions->warn(msg);

    Py_RETURN_NONE;
}


static PyObject *
Logger_error(PyObject *self, PyObject *args) {
    char *msg;

    if (!PyArg_ParseTuple(args, "s", &msg)) {
        return NULL;
    }
    ((LoggerObject*) self)->log_functions->error(msg);

    Py_RETURN_NONE;
}

static PyObject *
Logger_fatal(PyObject *self, PyObject *args) {
    char *msg;

    if (!PyArg_ParseTuple(args, "s", &msg)) {
        return NULL;
    }
    ((LoggerObject*) self)->log_functions->fatal(msg);

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

static int
import_python_module(const char *module_name) {
    PyObject *name = 0;

    name = PyUnicode_DecodeFSDefault(module_name);
    if (!name) {
        return (1);
    }
    hook_module = PyImport_Import(name);
    Py_DECREF(name);
    if (!hook_module) {
        return (1);
    }

    return (0);
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
    logger->log_functions = log_functions;
    // PyModule_AddObject steals reference on success
    if (PyModule_AddObject(kea_module, "logger", (PyObject*)logger) < 0) {
        Py_DECREF(&LoggerType);
        Py_DECREF(logger);
        return (1);
    }
    if (import_python_module(module_name)) {
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
