#include <boost/algorithm/string/predicate.hpp>
#include <hooks/hooks.h>
#include <log/message_initializer.h>
#include <log/macros.h>

#include "keapy_messages.h"

using namespace std;
using namespace isc::hooks;
using namespace isc::data;
using namespace isc::log;

extern "C" {
#include <stdlib.h>
#include <dlfcn.h>
#include <Python.h>
#include "../keamodule/keacapsule.h"

static Logger logger("keapy");

static void *libpython;

static PyObject *python_module;

void      (*dl_Py_Initialize)(void);
void      (*dl_Py_Finalize)(void);
PyObject* (*dl_PyUnicode_DecodeFSDefault)(const char *s);
PyObject* (*dl_PyImport_Import)(PyObject *name);
void      (*dl_Py_IncRef)(PyObject *);
void      (*dl_Py_DecRef)(PyObject *);
PyObject* (*dl_PyObject_GetAttrString)(PyObject *, const char *);
int       (*dl_PyList_Insert)(PyObject *, Py_ssize_t, PyObject *);
void*     (*dl_PyCapsule_Import)(const char *name, int no_block);

static int
find_symbol(void **sym, const char *name) {
    *sym = dlsym(libpython, name);
    if (*sym == 0) {
        LOG_ERROR(logger, LOG_KEAPY_HOOK).arg(string("symbol ").append(name).append(" not found"));
    }
    return *sym == 0;
}

#define load_symbol(name) find_symbol((void **)&dl_ ## name, #name)

static int
load_libpython(const char *name) {
    libpython = dlopen(name, RTLD_NOW | RTLD_GLOBAL);
    if (!libpython) {
        LOG_ERROR(logger, LOG_KEAPY_HOOK).arg(string("dlopen ").append(name).append(" failed"));
        return (1);
    }
    if (load_symbol(Py_Initialize)
        || load_symbol(Py_Finalize)
        || load_symbol(PyUnicode_DecodeFSDefault)
        || load_symbol(PyImport_Import)
        || load_symbol(Py_IncRef)
        || load_symbol(Py_DecRef)
        || load_symbol(PyObject_GetAttrString)
        || load_symbol(PyList_Insert)
        || load_symbol(PyCapsule_Import)) {
        return (1);
    }
    return (0);
}

static int
unload_libpython() {
    if (libpython) {
        dlclose(libpython);
        libpython = 0;
    }
    return (0);
}

static int
python_initialize() {
    dl_Py_Initialize();
    return (0);
}

static int
python_finalize() {
    if (dl_Py_Finalize) {
        dl_Py_Finalize();
    }
    return (0);
}

static void
log_debug(const char *msg) {
    LOG_DEBUG(logger, DBGLVL_TRACE_BASIC, LOG_KEAPY_PYTHON).arg(string(msg));
}

static void
log_info(const char *msg) {
    LOG_INFO(logger, LOG_KEAPY_PYTHON).arg(string(msg));
}

static void
log_warn(const char *msg) {
    LOG_WARN(logger, LOG_KEAPY_PYTHON).arg(string(msg));
}

static void
log_error(const char *msg) {
    LOG_ERROR(logger, LOG_KEAPY_PYTHON).arg(string(msg));
}

static void
log_fatal(const char *msg) {
    LOG_FATAL(logger, LOG_KEAPY_PYTHON).arg(string(msg));
}

static LogFunctions log_callbacks = {
    log_debug,
    log_info,
    log_warn,
    log_error,
    log_fatal
};

static int
load_kea_capsule(LibraryHandle &handle, string module_name) {
    kea_capsule = (void **)dl_PyCapsule_Import("kea._C_API", 0);
    if (!kea_capsule) {
        LOG_ERROR(logger, LOG_KEAPY_HOOK).arg("PyCapsule_Import(\"kea._C_API\") failed");
        return (1);
    }
    Kea_Bootstrap(&handle, &log_callbacks, module_name.c_str());
    return (0);
}

static int
unload_kea_capsule() {
    if (kea_capsule) {
        Kea_Shutdown();
    }
}

static PyObject *
make_python_string(const char *str) {
    PyObject *obj = dl_PyUnicode_DecodeFSDefault(str);
    if (!obj) {
        LOG_ERROR(logger, LOG_KEAPY_HOOK).arg(string("PyUnicode_DecodeFSDefault(\"").append(str).append("\") failed"));
    }
    return obj;
}

static int
split_module_path(const string module, string &module_path, string &module_name) {
    // extract the path portion of the module parameter
    size_t slash_pos = module.find_last_of('/');
    size_t basename_pos = 0;
    if (slash_pos != string::npos) {
        module_path = module.substr(0, slash_pos);
        basename_pos = slash_pos + 1;
    }
    // extract module basename by removing .py if it is present
    if (boost::algorithm::ends_with(module, ".py")) {
        module_name = module.substr(basename_pos, module.size() - basename_pos - 3);
    }
    else {
        module_name = module.substr(basename_pos, module.size() - basename_pos);
    }
    return (0);
}

static int
insert_python_path(const string module_path) {
    int res = 1;
    PyObject *sys = 0;
    PyObject *sys_module = 0;
    PyObject *path = 0;
    PyObject *cwd = 0;

    sys = make_python_string("sys");
    if (!sys) {
        goto error;
    }
    sys_module = dl_PyImport_Import(sys);
    if (!sys_module) {
        LOG_ERROR(logger, LOG_KEAPY_HOOK).arg("PyImport_Import(\"sys\") failed");
        goto error;
    }
    path = dl_PyObject_GetAttrString(sys_module, "path");
    if (!path) {
        LOG_ERROR(logger, LOG_KEAPY_HOOK).arg("PyObject_GetAttrString(sys_module, \"path\") failed");
        goto error;
    }
    cwd = dl_PyUnicode_DecodeFSDefault(module_path.c_str());
    if (dl_PyList_Insert(path, 0, cwd) < 0) {
        LOG_ERROR(logger, LOG_KEAPY_HOOK).arg("PyList_Insert(path, 0, cwd) failed");
        goto error;
    }
    res = 0;

error:
    dl_Py_DecRef(cwd);
    dl_Py_DecRef(path);
    dl_Py_DecRef(sys_module);
    dl_Py_DecRef(sys);
    return (res);
}

int
load(LibraryHandle &handle) {
    ConstElementPtr libpython  = handle.getParameter("libpython");
    ConstElementPtr module  = handle.getParameter("module");

    // check libpython parameter
    if (!libpython) {
        LOG_ERROR(logger, LOG_KEAPY_HOOK).arg("missing \"libpython\" parameter");
        return (1);
    }
    if (libpython->getType() != Element::string) {
        LOG_ERROR(logger, LOG_KEAPY_HOOK).arg("\"libpython\" parameter must be a string");
        return (1);
    }
    // check module parameter
    if (!module) {
        LOG_ERROR(logger, LOG_KEAPY_HOOK).arg("missing \"module\" parameter");
        return (1);
    }
    if (module->getType() != Element::string) {
        LOG_ERROR(logger, LOG_KEAPY_HOOK).arg("\"module\" parameter must be a string");
        return (1);
    }
    // split module into path and module
    string module_path;
    string module_name;
    split_module_path(module->stringValue(), module_path, module_name);

    // load libpython
    if (load_libpython(libpython->stringValue().c_str())) {
        return (1);
    }
    python_initialize();

    if (!module_path.empty() && insert_python_path(module_path)) {
        return (1);
    }
    // load the kea capsule
    if (load_kea_capsule(handle, module_name)) {
        return (1);
    }

    LOG_INFO(logger, LOG_KEAPY_HOOK).arg("keapyhook loaded");

    return (0);
}

int
unload() {
    unload_kea_capsule();
    python_finalize();
    unload_libpython();
    LOG_INFO(logger, LOG_KEAPY_HOOK).arg("keapyhook unloaded");
    return (0);
}

}
