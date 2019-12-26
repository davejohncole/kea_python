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

void      (*dl_Py_Initialize)(void);
void      (*dl_Py_Finalize)(void);
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
load_libpython(string name) {
    libpython = dlopen(name.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (!libpython) {
        LOG_ERROR(logger, LOG_KEAPY_HOOK).arg(string("dlopen ").append(name).append(" failed"));
        return (1);
    }
    if (load_symbol(Py_Initialize)
        || load_symbol(Py_Finalize)
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

static int
load_kea_capsule(LibraryHandle &handle, string module) {
    kea_capsule = (void **)dl_PyCapsule_Import("kea._C_API", 0);
    if (!kea_capsule) {
        LOG_ERROR(logger, LOG_KEAPY_HOOK).arg("PyCapsule_Import(\"kea._C_API\") failed");
        return (1);
    }
    Kea_SetLogger(logger, LOG_KEAPY_PYTHON);
    if (Kea_Bootstrap(&handle, module.c_str())) {
        LOG_ERROR(logger, LOG_KEAPY_HOOK).arg("Kea_Bootstrap failed");
        return (1);
    }
    return (0);
}

static int
unload_kea_capsule() {
    if (kea_capsule) {
        Kea_Shutdown();
    }
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

    if (load_libpython(libpython->stringValue())) {
        return (1);
    }
    python_initialize();
    if (load_kea_capsule(handle, module->stringValue())) {
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
