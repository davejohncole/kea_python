using namespace std;

extern "C" {
#include "keamodule.h"

PyObject *module;

static PyMethodDef kea_methods[] = {
    {NULL, NULL, 0, NULL}   /* sentinel */
};

static PyModuleDef kea_module = {
    PyModuleDef_HEAD_INIT,
    "kea",
    NULL,
    -1,
    kea_methods
};

PyMODINIT_FUNC
PyInit_kea(void) {
    module = PyModule_Create(&kea_module);
    if (module == NULL) {
        return NULL;
    }

    if (Logger_define_capsule(module)) {
        Py_DECREF(module);
        return NULL;
    }

    return module;
}

}
