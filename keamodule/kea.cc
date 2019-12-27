using namespace std;

extern "C" {
#include "keamodule.h"

PyObject *kea_module;

static PyMethodDef kea_methods[] = {
    {NULL, NULL, 0, NULL}   /* sentinel */
};

static PyModuleDef kea_module_def = {
    PyModuleDef_HEAD_INIT,
    "kea",
    NULL,
    -1,
    kea_methods
};

PyMODINIT_FUNC
PyInit_kea(void) {
    kea_module = PyModule_Create(&kea_module_def);
    if (kea_module == NULL) {
        return NULL;
    }

    if (Capsule_define()
        || Constants_define()) {
        Py_DECREF(kea_module);
        return NULL;
    }

    return kea_module;
}

}
