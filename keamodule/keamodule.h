#include <hooks/hooks.h>
#include <hooks/callout_manager.h>
#include <log/macros.h>

extern "C" {

#define PY_SSIZE_T_CLEAN
#include <Python.h>

// kea.cc
extern PyObject *kea_module;

// error.cc
extern int log_python_traceback();

// capsule.cc
extern isc::log::Logger *kea_logger;
extern isc::log::MessageID *kea_message_id;

extern int Capsule_define();

// constants.cc
extern int Constants_define();

// callouts.cc
extern int Callouts_define(isc::hooks::LibraryHandle *handle, PyObject *hook_module);

// library_handle.cc
typedef struct {
    PyObject_HEAD

    isc::hooks::LibraryHandle *handle;
} LibraryHandleObject;

extern int LibraryHandle_Check(PyObject *object);
extern PyObject *LibraryHandle_from_handle(isc::hooks::LibraryHandle *handle);
extern int LibraryHandle_define();

// callout_manager.cc
typedef struct {
    PyObject_HEAD

    isc::hooks::CalloutManager *manager;
} CalloutManagerObject;

extern int CalloutManager_Check(PyObject *object);
extern int CalloutManager_define();

}
