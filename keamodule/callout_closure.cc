#include "keamodule.h"

using namespace std;
using namespace isc::hooks;

extern "C" {

// Called when Kea invokes the bound callout.
// typedef int (*isc::hooks::CalloutPtr)(CalloutHandle &)
static void
CalloutClosure_binding(ffi_cif *cif, void *ret, void* args[], void *userdata) {
    PyObject *handle = 0;
    PyObject *result = 0;
    CalloutClosureObject *obj = (CalloutClosureObject *)userdata;
    int res = 1;

    // Kea calling us again - get the GIL.
    end_allow_threads();

    // REFCOUNT: CalloutHandle_from_handle - returns new reference
    handle = CalloutHandle_from_handle(*(CalloutHandle **)args[0]);
    if (!handle) {
        log_error("could not create CalloutHandle");
        goto error;
    }
    // REFCOUNT: PyObject_CallFunction - returns new reference
    result = PyObject_CallFunction(obj->callout, "O", handle);
    if (!result) {
        log_python_traceback();
        goto error;
    }
    if (!PyLong_CheckExact(result)) {
        // REFCOUNT: PyUnicode_AsUTF8 - returns UTF-8 encoding of str - buffer cached in str
        log_error(string(PyUnicode_AsUTF8(obj->name)) + " must return integer");
        goto error;
    }

    res = PyLong_AsLong(result);

error:
    *(ffi_arg *)ret = res;
    Py_XDECREF(result);
    Py_XDECREF(handle);

    // Going back into Kea - give up the GIL.
    begin_allow_threads();
}

// tp_dealloc - called when refcount is zero
static void
CalloutClosure_dealloc(CalloutClosureObject *self) {
    if (self->closure) {
        ffi_closure_free(self->closure);
    }
    Py_XDECREF(self->name);
    Py_XDECREF(self->callout);
    Py_TYPE(self)->tp_free((PyObject *) self);
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
    PyType_GenericNew                           // tp_new
};

PyObject *
CalloutClosure_from_object(PyObject *name, PyObject *callout) {
    // REFCOUNT: PyObject_New - returns new reference
    CalloutClosureObject *obj = PyObject_New(CalloutClosureObject, &CalloutClosureType);
    if (!obj) {
        return (0);
    }
    obj->closure = 0;
    obj->bound_callout = 0;
    obj->name = 0;
    obj->callout = 0;

    // see https://www.chiark.greenend.org.uk/doc/libffi-dev/html/Closure-Example.html
    // allocate memory for the closure in closure and save code address in bound_callout
    obj->closure = (ffi_closure *)ffi_closure_alloc(sizeof(ffi_closure), &obj->bound_callout);
    if (!obj->closure) {
        Py_DECREF(obj);
        return (PyErr_NoMemory());
    }
    // callout used one argument of type pointer
    obj->args[0] = &ffi_type_pointer;
    // prepare cif structure for use in ffi call
    int res = ffi_prep_cif(&obj->cif, FFI_DEFAULT_ABI, 1, &ffi_type_sint, obj->args);
    if (res != FFI_OK) {
        PyErr_Format(PyExc_RuntimeError, "ffi_prep_cif failed with result %d", res);
        Py_DECREF(obj);
        return (0);
    }
    // prepare the closure function to invoke CalloutClosure_binding with obj as the userdata
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

int
CalloutClosure_define() {
    // PyType_Ready - finish type initialisation
    if (PyType_Ready(&CalloutClosureType) < 0) {
        return (1);
    }

    return (0);
}

}
