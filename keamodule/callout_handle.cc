#include "keamodule.h"

using namespace std;
using namespace isc::hooks;
using namespace isc::dhcp;
using namespace isc::data;

extern "C" {

static PyObject *
CalloutHandle_getArgument(CalloutHandleObject *self, PyObject *args) {
    char *name;

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return (0);
    }

    if (strcmp(name, "lease4") == 0) {
        try {
            Lease4Ptr ptr;
            self->handle->getArgument(name, ptr);
            // REFCOUNT: Lease4_from_ptr - returns new reference
            return (Lease4_from_ptr(ptr));
        }
        catch (const exception &e) {
            PyErr_SetString(PyExc_TypeError, e.what());
            return (0);
        }
    }

    if (strcmp(name, "query4") == 0 || strcmp(name, "response4") == 0) {
        try {
            Pkt4Ptr ptr;
            self->handle->getArgument(name, ptr);
            // REFCOUNT: Pkt4_from_ptr - returns new reference
            return (Pkt4_from_ptr(ptr));
        }
        catch (const exception &e) {
            PyErr_SetString(PyExc_TypeError, e.what());
            return (0);
        }
    }

    if (strcmp(name, "command") == 0 || strcmp(name, "response") == 0) {
        try {
            ConstElementPtr ptr;
            self->handle->getArgument(name, ptr);
            // REFCOUNT: element_to_object - returns new reference
            return (element_to_object(ptr));
        }
        catch (const exception &e) {
            PyErr_SetString(PyExc_TypeError, e.what());
            return (0);
        }
    }

    PyErr_SetString(PyExc_ValueError, "unknown argument");
    return (0);
}

static PyObject *
CalloutHandle_setArgument(CalloutHandleObject *self, PyObject *args) {
    char *name;
    PyObject *value;

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
    if (!PyArg_ParseTuple(args, "sO", &name, &value)) {
        return (0);
    }

    if (strcmp(name, "lease4") == 0) {
        if (!Lease4_Check(value)) {
            PyErr_SetString(PyExc_TypeError, "expected Lease4 object");
            return (0);
        }
        try {
            self->handle->setArgument(name, ((Lease4Object *)value)->ptr);
            Py_RETURN_NONE;
        }
        catch (const exception &e) {
            PyErr_SetString(PyExc_TypeError, e.what());
            return (0);
        }
    }

    if (strcmp(name, "query4") == 0 || strcmp(name, "response4") == 0) {
        if (!Pkt4_Check(value)) {
            PyErr_SetString(PyExc_TypeError, "expected Pkt4 object");
            return (0);
        }
        try {
            self->handle->setArgument(name, ((Pkt4Object *)value)->ptr);
            Py_RETURN_NONE;
        }
        catch (const exception &e) {
            PyErr_SetString(PyExc_TypeError, e.what());
            return (0);
        }
    }

    if (strcmp(name, "command") == 0 || strcmp(name, "response") == 0) {
        try {
            ConstElementPtr ptr = object_to_element(value);
            if (!ptr) {
                return (0);
            }
            self->handle->setArgument(name, ptr);
            Py_RETURN_NONE;
        }
        catch (const exception &e) {
            PyErr_SetString(PyExc_TypeError, e.what());
            return (0);
        }
    }

    PyErr_SetString(PyExc_ValueError, "unknown argument");
    return (0);
}


ObjectHolder::ObjectHolder(PyObject *obj): obj_(obj) {
     Py_INCREF(obj);
}

ObjectHolder::~ObjectHolder() {
    // Kea caused this - get the GIL.
    end_allow_threads();

    Py_DECREF(obj_);

    // Going back into Kea - give up the GIL.
    begin_allow_threads();
}

static PyObject *
CalloutHandle_setContext(CalloutHandleObject *self, PyObject *args) {
    char *name;
    PyObject *value;

    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
    if (!PyArg_ParseTuple(args, "sO", &name, &value)) {
        return (0);
    }

    try {
        self->handle->setContext(name, ObjectHolderPtr(new ObjectHolder(value)));
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
CalloutHandle_getContext(CalloutHandleObject *self, PyObject *args) {
    char *name;

    if (!PyArg_ParseTuple(args, "s", &name)) {
        return (0);
    }

    try {
        ObjectHolderPtr ptr;
        self->handle->getContext(name, ptr);
        // REFCOUNT: return new reference to ObjectHolder for named value
        Py_INCREF(ptr->obj_);
        return (ptr->obj_);
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
CalloutHandle_deleteContext(CalloutHandleObject *self, PyObject *args) {
    char *name;

    if (!PyArg_ParseTuple(args, "s", &name)) {
        return (0);
    }

    try {
        self->handle->deleteContext(name);
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
CalloutHandle_getStatus(CalloutHandleObject *self, PyObject *args) {
    try {
        // REFCOUNT: PyLong_FromLong - returns new reference
        return PyLong_FromLong(self->handle->getStatus());
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyObject *
CalloutHandle_setStatus(CalloutHandleObject *self, PyObject *args) {
    int status;

    if (!PyArg_ParseTuple(args, "i", &status)) {
        return (0);
    }

    try {
        self->handle->setStatus(CalloutHandle::CalloutNextStep(status));
        Py_RETURN_NONE;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (0);
    }
}

static PyMethodDef CalloutHandle_methods[] = {
    {"getArgument", (PyCFunction) CalloutHandle_getArgument, METH_VARARGS,
     "Gets the value of an argument."},
    {"setArgument", (PyCFunction) CalloutHandle_setArgument, METH_VARARGS,
     "Sets the value of an argument."},
    {"setContext", (PyCFunction) CalloutHandle_setContext, METH_VARARGS,
     "Sets an element in the context associated with the current library."},
    {"getContext", (PyCFunction) CalloutHandle_getContext, METH_VARARGS,
     "Gets an element from the context associated with the current library."},
    {"deleteContext", (PyCFunction) CalloutHandle_deleteContext, METH_VARARGS,
     "Deletes an item of the given name from the context associated with the current library. If an item of that name does not exist, the method is a no-op."},
    {"getStatus", (PyCFunction) CalloutHandle_getStatus, METH_NOARGS,
     "Gets the next processing step."},
    {"setStatus", (PyCFunction) CalloutHandle_setStatus, METH_VARARGS,
     "Sets the next processing step."},
    {0}  // Sentinel
};

// tp_dealloc - called when refcount is zero
static void
CalloutHandle_dealloc(CalloutHandleObject *self) {
    if (self->is_owner) {
        delete self->handle;
    }
    Py_TYPE(self)->tp_free((PyObject *) self);
}

// tp_init - called after tp_new has returned an instance
static int
CalloutHandle_init(CalloutHandleObject *self, PyObject *args, PyObject *kwds) {
    PyObject *manager;

    if (kwds != 0) {
        PyErr_SetString(PyExc_TypeError, "keyword arguments are not supported");
        return (-1);
    }
    // REFCOUNT: PyArg_ParseTuple - returns borrowed references
    if (!PyArg_ParseTuple(args, "O!", &CalloutManagerType, &manager)) {
        return (-1);
    }

    try {
        self->handle = new CalloutHandle(((CalloutManagerObject *)manager)->manager);
        self->is_owner = true;
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (-1);
    }

    return (0);
}

// tp_new - allocate space and initialisation that can be repeated
static PyObject *
CalloutHandle_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    CalloutHandleObject *self;
    self = (CalloutHandleObject *) type->tp_alloc(type, 0);
    if (self) {
        self->handle = 0;
        self->is_owner = false;
    }
    return ((PyObject *) self);
}

PyTypeObject CalloutHandleType = {
    .ob_base = PyObject_HEAD_INIT(0)
    .tp_name = "kea.CalloutHandle",
    .tp_basicsize = sizeof(CalloutHandleObject),
    .tp_dealloc = (destructor) CalloutHandle_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = PyDoc_STR("Kea server CalloutHandle"),
    .tp_methods = CalloutHandle_methods,
    .tp_init = (initproc) CalloutHandle_init,
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = CalloutHandle_new,
};

PyObject *
CalloutHandle_from_handle(CalloutHandle *handle) {
    // REFCOUNT: PyObject_New - returns new reference
    CalloutHandleObject *obj = PyObject_New(CalloutHandleObject, &CalloutHandleType);
    if (obj) {
        obj->handle = handle;
        obj->is_owner = false;
    }
    return (PyObject *)obj;
}

int
CalloutHandle_define() {
    // PyType_Ready - finish type initialisation
    if (PyType_Ready(&CalloutHandleType) < 0) {
        return (1);
    }
    Py_INCREF(&CalloutHandleType);
    // REFCOUNT: PyModule_AddObject steals reference on success
    if (PyModule_AddObject(kea_module, "CalloutHandle", (PyObject *) &CalloutHandleType) < 0) {
        Py_DECREF(&CalloutHandleType);
        return (1);
    }

    return (0);
}

}
