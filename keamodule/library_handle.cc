#include <ffi.h>
#include "keamodule.h"

using namespace std;
using namespace isc::hooks;

extern "C" {

typedef struct {
    PyObject_HEAD

    PyObject *name;
    PyObject *callout;
    ffi_cif cif;
    ffi_type *args[1];
    void *bound_callout;
    ffi_closure *closure;
} CalloutClosureObject;

// typedef int (*isc::hooks::CalloutPtr)(CalloutHandle &)
static void
CalloutClosure_binding(ffi_cif *cif, void *ret, void* args[], void *userdata) {
    PyObject *handle = 0;
    PyObject *result = 0;
    CalloutClosureObject *obj = (CalloutClosureObject *)userdata;
    int res = 1;

    handle = CalloutHandle_from_handle(*(CalloutHandle**)args[0]);
    if (!handle) {
        log_error("could not create CalloutHandle");
        goto error;
    }
    result = PyObject_CallFunction(obj->callout, "O", handle);
    if (!result) {
        log_python_traceback();
        goto error;
    }
    if (!PyLong_CheckExact(result)) {
        log_error(string(PyUnicode_AsUTF8(obj->name)) + " must return integer");
        goto error;
    }

    res = PyLong_AsLong(result);

error:
    *(ffi_arg *)ret = res;
    Py_XDECREF(result);
    Py_XDECREF(handle);
}

static void
CalloutClosure_dealloc(CalloutClosureObject *self) {
    if (self->closure) {
        ffi_closure_free(self->closure);
    }
    Py_XDECREF(self->name);
    Py_XDECREF(self->callout);
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *
CalloutClosure_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    return (type->tp_alloc(type, 0));
}

PyTypeObject CalloutClosureType = {
    PyObject_HEAD_INIT(0)
    "kea.CalloutClosure",                       // tp_name
    sizeof(CalloutClosureObject),               // tp_basicsize
    0,                                          // tp_itemsize
    (destructor) CalloutClosure_dealloc,        // tp_dealloc
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
    "Kea server callout closure",               // tp_doc
    0,                                          // tp_traverse
    0,                                          // tp_clear
    0,                                          // tp_richcompare
    0,                                          // tp_weaklistoffset
    0,                                          // tp_iter
    0,                                          // tp_iternext
    0,                                          // tp_methods
    0,                                          // tp_members
    0,                                          // tp_getset
    0,                                          // tp_base
    0,                                          // tp_dict
    0,                                          // tp_descr_get
    0,                                          // tp_descr_set
    0,                                          // tp_dictoffset
    0,                                          // tp_init
    PyType_GenericAlloc,                        // tp_alloc
    CalloutClosure_new                          // tp_new
};

PyObject *
CalloutClosure_from_object(PyObject *name, PyObject *callout) {
    CalloutClosureObject *obj = PyObject_New(CalloutClosureObject, &CalloutClosureType);
    if (!obj) {
        return (0);
    }
    obj->closure = 0;
    obj->bound_callout = 0;
    obj->name = 0;
    obj->callout = 0;

    obj->closure = (ffi_closure *)ffi_closure_alloc(sizeof(ffi_closure), &obj->bound_callout);
    if (!obj->closure) {
        Py_DECREF(obj);
        return (PyErr_NoMemory());
    }
    obj->args[0] = &ffi_type_pointer;
    int res = ffi_prep_cif(&obj->cif, FFI_DEFAULT_ABI, 1, &ffi_type_sint, obj->args);
    if (res != FFI_OK) {
        PyErr_Format(PyExc_RuntimeError, "ffi_prep_cif failed with result %d", res);
        Py_DECREF(obj);
        return (0);
    }
    res = ffi_prep_closure_loc(obj->closure, &obj->cif, CalloutClosure_binding, obj, obj->bound_callout);
    if (res != FFI_OK) {
        PyErr_Format(PyExc_RuntimeError, "ffi_prep_closure_loc failed with result %d", res);
        Py_DECREF(obj);
        return (0);
    }
    Py_INCREF(name);
    Py_INCREF(callout);
    obj->name = name;
    obj->callout = callout;

    return ((PyObject *) obj);
}

static PyObject *
LibraryHandle_registerCommandCallout(PyObject *self, PyObject *args) {
    PyObject *name;
    PyObject *callout;

    if (!PyArg_ParseTuple(args, "O!O", &PyUnicode_Type, &name, &callout)) {
        return (0);
    }
    if (!PyCallable_Check(callout)) {
        PyErr_SetString(PyExc_TypeError, "callout must be callable");
        return (0);
    }

    CalloutClosureObject *obj = (CalloutClosureObject *) CalloutClosure_from_object(name, callout);
    if (!obj) {
        return (0);
    }
    ((LibraryHandleObject *)self)->handle->registerCommandCallout(PyUnicode_AsUTF8(name), (CalloutPtr)obj->bound_callout);

    Py_RETURN_NONE;
}

static PyMethodDef LibraryHandle_methods[] = {
    {"registerCommandCallout", (PyCFunction) LibraryHandle_registerCommandCallout, METH_VARARGS,
     "Register control command handler."},
    {0}  // Sentinel
};

static int
LibraryHandle_init(LibraryHandleObject *self, PyObject *args, PyObject *kwds) {
    PyObject *manager = 0;

    if (kwds != 0) {
        PyErr_SetString(PyExc_TypeError, "keyword arguments are not supported");
        return (0);
    }
    if (!PyArg_ParseTuple(args, "O!", &CalloutManagerType, &manager)) {
        return (-1);
    }

    try {
        self->handle = new LibraryHandle(((CalloutManagerObject*)manager)->manager.get());
    }
    catch (const exception &e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return (-1);
    }

    return (0);
}

static PyObject *
LibraryHandle_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    LibraryHandleObject *self;
    self = (LibraryHandleObject *) type->tp_alloc(type, 0);
    if (self) {
        self->handle = 0;
    }
    return ((PyObject *) self);
}

PyTypeObject LibraryHandleType = {
    PyObject_HEAD_INIT(0)
    "kea.LibraryHandle",                        // tp_name
    sizeof(LibraryHandleObject),                // tp_basicsize
    0,                                          // tp_itemsize
    0,                                          // tp_dealloc
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
    "Kea server LibraryHandle",                 // tp_doc
    0,                                          // tp_traverse
    0,                                          // tp_clear
    0,                                          // tp_richcompare
    0,                                          // tp_weaklistoffset
    0,                                          // tp_iter
    0,                                          // tp_iternext
    LibraryHandle_methods,                      // tp_methods
    0,                                          // tp_members
    0,                                          // tp_getset
    0,                                          // tp_base
    0,                                          // tp_dict
    0,                                          // tp_descr_get
    0,                                          // tp_descr_set
    0,                                          // tp_dictoffset
    (initproc) LibraryHandle_init,              // tp_init
    PyType_GenericAlloc,                        // tp_alloc
    LibraryHandle_new                           // tp_new
};

PyObject *
LibraryHandle_from_handle(LibraryHandle *handle) {
    LibraryHandleObject *obj = PyObject_New(LibraryHandleObject, &LibraryHandleType);
    if (obj) {
        obj->handle = handle;
    }
    return (PyObject *)obj;
}

int
LibraryHandle_define() {
    if (PyType_Ready(&CalloutClosureType) < 0) {
        return (1);
    }
    if (PyType_Ready(&LibraryHandleType) < 0) {
        return (1);
    }
    Py_INCREF(&LibraryHandleType);
    if (PyModule_AddObject(kea_module, "LibraryHandle", (PyObject *) &LibraryHandleType) < 0) {
        Py_DECREF(&LibraryHandleType);
        return (1);
    }

    return (0);
}

}
