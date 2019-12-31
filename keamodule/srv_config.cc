#include "keamodule.h"
#include <dhcpsrv/cfgmgr.h>

using namespace std;
using namespace isc::hooks;
using namespace isc::dhcp;
using namespace isc::data;

extern "C" {

PyObject *
element_to_object(ConstElementPtr ptr) {
    switch (ptr->getType()) {
        case Element::integer:
            try {
                return (PyLong_FromLong(ptr->intValue()));
            }
            catch (const exception &e) {
                PyErr_SetString(PyExc_TypeError, e.what());
                return (0);
            }

        case Element::real:
            try {
                return (PyFloat_FromDouble(ptr->doubleValue()));
            }
            catch (const exception &e) {
                PyErr_SetString(PyExc_TypeError, e.what());
                return (0);
            }

        case Element::boolean:
            try {
                return (PyBool_FromLong(ptr->boolValue()));
            }
            catch (const exception &e) {
                PyErr_SetString(PyExc_TypeError, e.what());
                return (0);
            }

        case Element::null:
            Py_RETURN_NONE;

        case Element::string:
            try {
                return (PyUnicode_FromString(ptr->stringValue().c_str()));
            }
            catch (const exception &e) {
                PyErr_SetString(PyExc_TypeError, e.what());
                return (0);
            }

        case Element::list: {
            auto list_ptr = ptr->listValue();
            PyObject *list = PyList_New(list_ptr.size());
            if (!list) {
                return (0);
            }
            try {
                for (std::vector<ElementPtr>::const_iterator it = list_ptr.begin(); it != list_ptr.cend(); ++it) {
                    auto pos = it - list_ptr.begin() ;
                    PyObject *elem = element_to_object(*it);
                    if (!elem || PyList_SetItem(list, pos, elem) < 0) {
                        Py_DECREF(list);
                        return (0);
                    }
                }
            }
            catch (const exception &e) {
                PyErr_SetString(PyExc_TypeError, e.what());
                Py_DECREF(list);
                return (0);
            }
            return (list);
        }

        case Element::map: {
            auto map_ptr = ptr->mapValue();
            PyObject *dict = PyDict_New();
            if (!dict) {
                return (0);
            }
            try {
                for (std::map<std::string, ConstElementPtr>::const_iterator it = map_ptr.begin(); it != map_ptr.end(); ++it) {
                    PyObject *elem = element_to_object(it->second);
                    if (!elem || PyDict_SetItemString(dict, it->first.c_str(), elem) < 0) {
                        Py_DECREF(dict);
                        return (0);
                    }
                }
            }
            catch (const exception &e) {
                PyErr_SetString(PyExc_TypeError, e.what());
                Py_DECREF(dict);
                return (0);
            }
            return (dict);
        }

        default:
            Py_RETURN_NONE;
    }
}

static PyObject *
SrvConfig_toElement(PyObject *self, PyObject *args) {
    try {
        ElementPtr ptr = ((SrvConfigObject *)self)->ptr->toElement();
        return (element_to_object(ptr));
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyMethodDef SrvConfig_methods[] = {
    {"toElement", (PyCFunction) SrvConfig_toElement, METH_NOARGS,
     "Unparse configuration object."},
    {0}  // Sentinel
};

static PyObject *
SrvConfig_use_count(OptionObject *self, void *closure) {
    return (PyLong_FromLong(self->ptr.use_count()));
}

static PyGetSetDef SrvConfig_getsetters[] = {
    {(char *)"use_count", (getter) SrvConfig_use_count, (setter) 0, (char *)"shared_ptr use count", 0},
    {0}  // Sentinel
};

static void
SrvConfig_dealloc(SrvConfigObject *self) {
    self->ptr.reset();
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static int
SrvConfig_init(SrvConfigObject *self, PyObject *args, PyObject *kwds) {
    char *config = (char *)"current";
 
    if (kwds != 0) {
        PyErr_SetString(PyExc_TypeError, "keyword arguments are not supported");
        return (0);
    }
    if (!PyArg_ParseTuple(args, "|s", &config)) {
        return (-1);
    }

    try {
        if (strcmp(config, "staging") == 0) {
            self->ptr = CfgMgr::instance().getStagingCfg();
        }
        else if (strcmp(config, "current") == 0) {
            self->ptr = CfgMgr::instance().getCurrentCfg();
        }
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (-1);
    }

    return (0);
}

static PyObject *
SrvConfig_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    SrvConfigObject *self;
    self = (SrvConfigObject *) type->tp_alloc(type, 0);
    if (self) {
        self->ptr.reset();
    }
    return ((PyObject *) self);
}

PyTypeObject SrvConfigType = {
    PyObject_HEAD_INIT(0)
    "kea.SrvConfig",                            // tp_name
    sizeof(SrvConfigObject),                    // tp_basicsize
    0,                                          // tp_itemsize
    (destructor) SrvConfig_dealloc,             // tp_dealloc
    0,                                          // tp_vectorcall_offset
    0,                                          // tp_getattr
    0,                                          // tp_setattr
    0,                                          // tp_as_async
    0,                                          // tp_repr
    0,                                          // tp_as_number
    0,                                          // tp_as_sequence
    0,                                          // tp_as_mapping
    0,                                          // tp_hash
    0,                                          // tp_call
    0,                                          // tp_str
    0,                                          // tp_getattro
    0,                                          // tp_setattro
    0,                                          // tp_as_buffer
    Py_TPFLAGS_DEFAULT,                         // tp_flags
    "Kea server SrvConfig",                     // tp_doc
    0,                                          // tp_traverse
    0,                                          // tp_clear
    0,                                          // tp_richcompare
    0,                                          // tp_weaklistoffset
    0,                                          // tp_iter
    0,                                          // tp_iternext
    SrvConfig_methods,                          // tp_methods
    0,                                          // tp_members
    SrvConfig_getsetters,                       // tp_getset
    0,                                          // tp_base
    0,                                          // tp_dict
    0,                                          // tp_descr_get
    0,                                          // tp_descr_set
    0,                                          // tp_dictoffset
    (initproc) SrvConfig_init,                  // tp_init
    PyType_GenericAlloc,                        // tp_alloc
    SrvConfig_new                               // tp_new
};

int
SrvConfig_define() {
    if (PyType_Ready(&SrvConfigType) < 0) {
        return (1);
    }
    Py_INCREF(&SrvConfigType);
    if (PyModule_AddObject(kea_module, "SrvConfig", (PyObject *) &SrvConfigType) < 0) {
        Py_DECREF(&SrvConfigType);
        return (1);
    }

    return (0);
}

}
