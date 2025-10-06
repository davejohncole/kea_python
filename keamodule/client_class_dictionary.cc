#include "keamodule.h"

using namespace std;
using namespace isc::dhcp;

extern "C" {

static PyObject *
ClientClassDictionary_getClasses(ClientClassDictionaryObject *self, PyObject *Py_UNUSED(ignored)) {
    try {
        const ClientClassDefListPtr& classes = self->ptr->getClasses();
        // REFCOUNT: PyList_New - returns new reference
        PyObject *list = PyList_New(0);
        if (!list) {
            return (0);
        }
        
        for (const auto& class_def : *classes) {
            // REFCOUNT: ClientClassDef_from_ptr - returns new reference
            PyObject *obj = ClientClassDef_from_ptr(const_cast<ClientClassDefPtr&>(class_def));
            if (!obj) {
                // REFCOUNT: Py_DECREF - decrements reference count
                Py_DECREF(list);
                return (0);
            }
            // REFCOUNT: PyList_Append - does not steal reference
            if (PyList_Append(list, obj) < 0) {
                // REFCOUNT: Py_DECREF - decrements reference count
                Py_DECREF(obj);
                Py_DECREF(list);
                return (0);
            }
            // REFCOUNT: Py_DECREF - decrements reference count (PyList_Append doesn't steal)
            Py_DECREF(obj);
        }
        
        return list;
    }
    catch (const exception &e) {
        // REFCOUNT: PyErr_SetString - does not affect reference counts
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyMethodDef ClientClassDictionary_methods[] = {
    {"getClasses", (PyCFunction) ClientClassDictionary_getClasses, METH_NOARGS,
     "Returns a list of ClientClassDef objects."},
    {0}  // Sentinel
};

// tp_dealloc - called when refcount is zero
static void
ClientClassDictionary_dealloc(ClientClassDictionaryObject *self) {
    self->ptr.~ClientClassDictionaryPtr();
    // REFCOUNT: tp_free - frees object
    Py_TYPE(self)->tp_free((PyObject *) self);
}

// tp_init - called after tp_new has returned an instance
static int
ClientClassDictionary_init(ClientClassDictionaryObject *self, PyObject *args, PyObject *kwds) {
    if (kwds != 0) {
        // REFCOUNT: PyErr_SetString - does not affect reference counts
        PyErr_SetString(PyExc_TypeError, "keyword arguments are not supported");
        return (-1);
    }
    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
    if (!PyArg_ParseTuple(args, "")) {
        return (-1);
    }

    return (0);
}

// tp_new - allocate space and initialisation that can be repeated
static PyObject *
ClientClassDictionary_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    ClientClassDictionaryObject *self;
    // REFCOUNT: tp_alloc - returns new reference
    self = (ClientClassDictionaryObject *) type->tp_alloc(type, 0);
    new (&self->ptr) ClientClassDictionaryPtr(boost::make_shared<ClientClassDictionary>());
    return ((PyObject *) self);
}

PyTypeObject ClientClassDictionaryType = {
    .ob_base = PyObject_HEAD_INIT(0)
    .tp_name = "kea.ClientClassDictionary",
    .tp_basicsize = sizeof(ClientClassDictionaryObject),
    .tp_dealloc = (destructor) ClientClassDictionary_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Kea server ClientClassDictionary"),
    .tp_methods = ClientClassDictionary_methods,
    .tp_init = (initproc) ClientClassDictionary_init,
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = ClientClassDictionary_new,
};

int
ClientClassDictionary_define() {
    // PyType_Ready - finish type initialisation
    if (PyType_Ready(&ClientClassDictionaryType) < 0) {
        return (1);
    }
    Py_INCREF(&ClientClassDictionaryType);
    // REFCOUNT: PyModule_AddObject steals reference on success
    if (PyModule_AddObject(kea_module, "ClientClassDictionary", (PyObject *) &ClientClassDictionaryType) < 0) {
        Py_DECREF(&ClientClassDictionaryType);
        return (1);
    }

    return (0);
}

}