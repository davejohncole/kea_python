#include <boost/algorithm/string/predicate.hpp>
#include <hooks/hooks.h>
#include <log/message_initializer.h>
#include <log/macros.h>

using namespace isc::hooks;
using namespace isc::log;
using namespace std;

extern "C" {
#include "keamodule.h"
#define KEA_MODULE
#include "keacapsule.h"

static PyObject *hook_module;
static PyObject *traceback_module;
static Logger *kea_logger = 0;
static MessageID *kea_message_id = 0;

typedef struct {
    PyObject_HEAD
} LoggerObject;

static PyObject *
Logger_debug(PyObject *self, PyObject *args) {
    char *msg;

    if (!PyArg_ParseTuple(args, "s", &msg)) {
        return NULL;
    }
    LOG_DEBUG(*kea_logger, DBGLVL_TRACE_BASIC, *kea_message_id).arg(string(msg));

    Py_RETURN_NONE;
}

static PyObject *
Logger_info(PyObject *self, PyObject *args) {
    char *msg;

    if (!PyArg_ParseTuple(args, "s", &msg)) {
        return NULL;
    }
    LOG_INFO(*kea_logger, *kea_message_id).arg(string(msg));

    Py_RETURN_NONE;
}

static PyObject *
Logger_warn(PyObject *self, PyObject *args) {
    char *msg;

    if (!PyArg_ParseTuple(args, "s", &msg)) {
        return NULL;
    }
    LOG_WARN(*kea_logger, *kea_message_id).arg(string(msg));

    Py_RETURN_NONE;
}


static PyObject *
Logger_error(PyObject *self, PyObject *args) {
    char *msg;

    if (!PyArg_ParseTuple(args, "s", &msg)) {
        return NULL;
    }
    LOG_ERROR(*kea_logger, *kea_message_id).arg(string(msg));

    Py_RETURN_NONE;
}

static PyObject *
Logger_fatal(PyObject *self, PyObject *args) {
    char *msg;

    if (!PyArg_ParseTuple(args, "s", &msg)) {
        return NULL;
    }
    LOG_FATAL(*kea_logger, *kea_message_id).arg(string(msg));

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
        traceback_module = PyImport_ImportModule("traceback");
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
    LOG_ERROR(*kea_logger, *kea_message_id).arg(string(PyUnicode_AsUTF8(formatted_error)));

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

static void
Kea_SetLogger(Logger &logger, MessageID& ident) {
    kea_logger = &logger;
    kea_message_id = &ident;
}

static int
split_module_path(const string module, string &module_path, string &module_name) {
    // extract the path portion of the module parameter
    size_t slash_pos = module.find_last_of('/');
    size_t basename_pos = 0;
    if (slash_pos != string::npos) {
        module_path = module.substr(0, slash_pos);
        basename_pos = slash_pos + 1;
    }
    // extract module basename by removing .py if it is present
    if (boost::algorithm::ends_with(module, ".py")) {
        module_name = module.substr(basename_pos, module.size() - basename_pos - 3);
    }
    else {
        module_name = module.substr(basename_pos, module.size() - basename_pos);
    }
    return (0);
}

static int
insert_python_path(const string module_path) {
    int res = 1;
    PyObject *sys_module = 0;
    PyObject *path = 0;
    PyObject *cwd = 0;

    sys_module = PyImport_ImportModule("sys");
    if (!sys_module) {
        LOG_ERROR(*kea_logger, *kea_message_id).arg("PyImport_ImportModule(\"sys\") failed");
        goto error;
    }
    path = PyObject_GetAttrString(sys_module, "path");
    if (!path) {
        LOG_ERROR(*kea_logger, *kea_message_id).arg("PyObject_GetAttrString(sys_module, \"path\") failed");
        goto error;
    }
    cwd = PyUnicode_DecodeFSDefault(module_path.c_str());
    if (PyList_Insert(path, 0, cwd) < 0) {
        LOG_ERROR(*kea_logger, *kea_message_id).arg("PyList_Insert(path, 0, cwd) failed");
        goto error;
    }
    res = 0;

error:
    Py_XDECREF(cwd);
    Py_XDECREF(path);
    Py_XDECREF(sys_module);

    return (res);
}

static int
Kea_Bootstrap(LibraryHandle *handle, const char *module) {
    // split module into path and module
    string module_path;
    string module_name;
    split_module_path(module, module_path, module_name);

    if (!module_path.empty() && insert_python_path(module_path)) {
        return (1);
    }

    if (PyType_Ready(&LoggerType) < 0) {
        return (1);
    }
    Py_INCREF(&LoggerType);
    LoggerObject *logger = PyObject_New(LoggerObject, &LoggerType);
    if (!logger) {
        return (1);
    }
    // PyModule_AddObject steals reference on success
    if (PyModule_AddObject(kea_module, "logger", (PyObject*)logger) < 0) {
        Py_DECREF(logger);
        return (1);
    }
    hook_module = PyImport_ImportModule(module_name.c_str());
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
Capsule_define() {
    static void *kea_capsule[Kea_API_pointers];
    PyObject *c_api_object = 0;

    // initialize the C API pointer array
    kea_capsule[Kea_SetLogger_NUM] = (void *)Kea_SetLogger;
    kea_capsule[Kea_Bootstrap_NUM] = (void *)Kea_Bootstrap;
    kea_capsule[Kea_Shutdown_NUM] = (void *)Kea_Shutdown;
    // create a Capsule containing the API pointer array's address
    c_api_object = PyCapsule_New((void *)kea_capsule, "kea._C_API", NULL);
    if (PyModule_AddObject(kea_module, "_C_API", c_api_object) < 0) {
        Py_XDECREF(c_api_object);
        return (1);
    }
    // initialize logger of None - will be replaced with Capsule Kea_Bootstrap
    Py_INCREF(Py_None);
    if (PyModule_AddObject(kea_module, "logger", Py_None) < 0) {
        Py_DECREF(Py_None);
        return (1);
    }

    return (0);
}

}
