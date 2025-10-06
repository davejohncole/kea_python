#include "keamodule.h"
#include <dhcpsrv/parsers/client_class_def_parser.h>

using namespace std;
using namespace isc::dhcp;
using namespace isc::data;

extern "C" {

static PyObject *
ClientClassDefParser_parse(ClientClassDefParserObject *self, PyObject *args) {
    ClientClassDictionaryObject *class_dictionary_obj;
    PyObject *config;
    int family = AF_INET;

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
    if (!PyArg_ParseTuple(args, "O!O|i", &ClientClassDictionaryType, &class_dictionary_obj, &config, &family)) {
        return (0);
    }
    
    try {
        ElementPtr data = object_to_element(config);
        
        self->ptr->parse(class_dictionary_obj->ptr, data, family);
        
        // Return None - the class is added to the dictionary
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        // REFCOUNT: PyErr_SetString - does not affect reference counts
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyMethodDef ClientClassDefParser_methods[] = {
    {"parse", (PyCFunction) ClientClassDefParser_parse, METH_VARARGS,
     "Parses a single client class definition and adds to the class dictionary."},
    {0}  // Sentinel
};

// tp_dealloc - called when refcount is zero
static void
ClientClassDefParser_dealloc(ClientClassDefParserObject *self) {
    self->ptr.~ClientClassDefParserPtr();
    Py_TYPE(self)->tp_free((PyObject *) self);
}

// tp_init - called after tp_new has returned an instance
static int
ClientClassDefParser_init(ClientClassDefParserObject *self, PyObject *args, PyObject *kwds) {
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
ClientClassDefParser_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    ClientClassDefParserObject *self;
    // REFCOUNT: tp_alloc - returns new reference
    self = (ClientClassDefParserObject *) type->tp_alloc(type, 0);
    new (&self->ptr) ClientClassDefParserPtr(boost::make_shared<ClientClassDefParser>());
    return ((PyObject *) self);
}

PyTypeObject ClientClassDefParserType = {
    .ob_base = PyObject_HEAD_INIT(0)
    .tp_name = "kea.ClientClassDefParser",
    .tp_basicsize = sizeof(ClientClassDefParserObject),
    .tp_dealloc = (destructor) ClientClassDefParser_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Kea server ClientClassDefParser"),
    .tp_methods = ClientClassDefParser_methods,
    .tp_init = (initproc) ClientClassDefParser_init,
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = ClientClassDefParser_new,
};

int
ClientClassDefParser_define() {
    // PyType_Ready - finish type initialisation
    if (PyType_Ready(&ClientClassDefParserType) < 0) {
        return (1);
    }
    Py_INCREF(&ClientClassDefParserType);
    // REFCOUNT: PyModule_AddObject steals reference on success
    if (PyModule_AddObject(kea_module, "ClientClassDefParser", (PyObject *) &ClientClassDefParserType) < 0) {
        Py_DECREF(&ClientClassDefParserType);
        return (1);
    }

    return (0);
}

}