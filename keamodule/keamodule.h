extern "C" {

#define PY_SSIZE_T_CLEAN
#include <Python.h>

extern PyObject *kea_module;

extern int Logger_define_capsule(PyObject *module);

}
