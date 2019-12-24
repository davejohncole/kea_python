using namespace std;

extern "C" {
#include <Python.h>
#define KEA_MODULE
#include "kea_capsule.h"

static LogFunctions *log_callbacks;

static int Kea_Bootstrap(LogFunctions *log_functions) {
    log_callbacks = log_functions;
    return (0);
}

static int Kea_Shutdown() {
    log_callbacks = NULL;
    return (0);
}

static PyObject *
kea_debug(PyObject *self, PyObject *args) {
    char *msg;

    if (!PyArg_ParseTuple(args, "s", &msg)) {
        return NULL;
    }
    if (log_callbacks) {
        log_callbacks->debug(msg);
    }

    Py_RETURN_NONE;
}

static PyObject *
kea_info(PyObject *self, PyObject *args) {
    char *msg;

    if (!PyArg_ParseTuple(args, "s", &msg)) {
        return NULL;
    }
    if (log_callbacks) {
        log_callbacks->info(msg);
    }

    Py_RETURN_NONE;
}

static PyMethodDef kea_methods[] = {
    {"debug", (PyCFunction)(void(*)(void))kea_debug, METH_VARARGS,
     "Log a debug message to the kea logger."},
    {"info", (PyCFunction)(void(*)(void))kea_info, METH_VARARGS,
     "Log an info message to the kea logger."},
    {NULL, NULL, 0, NULL}   /* sentinel */
};

PyModuleDef kea_module = {
    PyModuleDef_HEAD_INIT,
    "kea",
    NULL,
    -1,
    kea_methods
};

PyMODINIT_FUNC
PyInit_kea(void) {
    PyObject *m;
    static void *kea_capsule[Kea_API_pointers];
    PyObject *c_api_object;

    m = PyModule_Create(&kea_module);
    if (m == NULL) {
        return NULL;
    }
    /* Initialize the C API pointer array */
    kea_capsule[Kea_Bootstrap_NUM] = (void *)Kea_Bootstrap;
    kea_capsule[Kea_Shutdown_NUM] = (void *)Kea_Shutdown;

    /* Create a Capsule containing the API pointer array's address */
    c_api_object = PyCapsule_New((void *)kea_capsule, "kea._C_API", NULL);

    if (PyModule_AddObject(m, "_C_API", c_api_object) < 0) {
        Py_XDECREF(c_api_object);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}

}
