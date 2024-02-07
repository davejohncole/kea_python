#include "keamodule.h"

using namespace std;

extern "C" {

PyObject *kea_module;

static PyMethodDef kea_methods[] = {
    {0, 0, 0, 0}   // sentinel
};

static PyModuleDef kea_module_def = {
    PyModuleDef_HEAD_INIT,
    "kea",
    0,
    -1,
    kea_methods
};

PyMODINIT_FUNC
PyInit_kea(void) {
    kea_module = PyModule_Create(&kea_module_def);
    if (!kea_module) {
        return (0);
    }

    if (PyModule_AddStringConstant(kea_module, "__version__", VERSION) < 0
        || Capsule_define()
        || Constants_define()
        || CalloutClosure_define()
        || LibraryHandle_define()
        || CalloutManager_define()
        || CalloutHandle_define()
        || Lease4_define()
        || Pkt4_define()
        || Option_define()
        || CfgMgr_define()
        || SrvConfig_define()
        || CfgSubnets4_define()
        || Subnet4_define()
        || Subnet4ConfigParser_define()
        || LeaseMgr_define()
        || Host_define()
        || HostMgr_define()
        || HostReservationParser4_define()
        || ConfigBackendDHCPv4Mgr_define()
        || ConfigBackendPoolDHCPv4_define()) {
        Py_DECREF(kea_module);
        return (0);
    }

    return (kea_module);
}

}
