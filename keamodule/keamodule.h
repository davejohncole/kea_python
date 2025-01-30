#include <ffi.h>
#include <config.h>
#include <hooks/hooks.h>
#include <hooks/callout_manager.h>
#include <log/macros.h>
#include <log/logger_manager.h>
#include <dhcpsrv/lease.h>
#include <dhcpsrv/subnet.h>
#include <dhcp/pkt4.h>
#include <dhcpsrv/cfgmgr.h>
#include <dhcpsrv/lease_mgr.h>
#include <dhcpsrv/host_mgr.h>
#include <dhcp/dhcp4.h>
#include <dhcpsrv/parsers/dhcp_parsers.h>
#include <dhcpsrv/parsers/host_reservation_parser.h>
#include <dhcpsrv/config_backend_dhcp4_mgr.h>
#include <database/backend_selector.h>

extern "C" {

#define PY_SSIZE_T_CLEAN
#include <Python.h>

// kea.cc
extern PyObject *kea_module;

// errors.cc
extern int Errors_initialize();
extern int Errors_finalize();
// steals reference to exc_* objects
extern int format_python_traceback(PyObject *exc_type, PyObject *exc_value, PyObject *exc_traceback, std::string &traceback);
extern int log_python_traceback();

// CalloutClosureObject contains a FFI callout that binds to a python function.
// Objects are defined in callout_closure.cc, but instances are created and
// managed in callouts.cc
typedef struct {
    PyObject_HEAD

    PyObject *name;         // name of callout
    PyObject *callout;      // python function to call for callout
    ffi_cif cif;            // ffi_prep_cif return value (via pointer arg)
    ffi_type *args[1];      // describe ffi function argument
    void *bound_callout;    // ffi_closure_alloc return value (via pointer arg) - the function to call
    ffi_closure *closure;   // ffi_closure_alloc return value - function to call
} CalloutClosureObject;

// callouts.cc
extern int Callouts_add_closure(CalloutClosureObject *obj);
extern int Callouts_register(isc::hooks::LibraryHandle *handle);
extern int Callouts_unregister();

// callout_closure.cc
extern PyObject *CalloutClosure_from_object(PyObject *name, PyObject *callout);
extern int CalloutClosure_define();

// callout_handle.cc
typedef struct {
    PyObject_HEAD

    isc::hooks::CalloutHandle *handle;
    bool is_owner;                          // tracks whether we created the handle or not
} CalloutHandleObject;

// wrap a Python object in a boost shared_ptr so that when the C++ object is freed the Python object is DECREF'ed
class ObjectHolder {
public:
    ObjectHolder(PyObject *obj);
    ~ObjectHolder();
    PyObject *obj_;
};
typedef boost::shared_ptr<ObjectHolder> ObjectHolderPtr;

#define CalloutHandle_Check(op) (Py_TYPE(op) == &CalloutHandleType)
extern PyTypeObject CalloutHandleType;
extern PyObject *CalloutHandle_from_handle(isc::hooks::CalloutHandle *handle);
extern int CalloutHandle_define();

// callout_manager.cc
typedef boost::shared_ptr<isc::hooks::CalloutManager> CalloutManagerPtr;
typedef struct {
    PyObject_HEAD

    CalloutManagerPtr manager;
} CalloutManagerObject;

#define CalloutManager_Check(op) (Py_TYPE(op) == &CalloutManagerType)
extern PyTypeObject CalloutManagerType;
extern int CalloutManager_define();

// capsule.cc
extern PyObject *hook_module;
extern isc::log::Logger *kea_logger;
extern isc::log::MessageID *kea_message_id;
// call begin_allow_threads when going back into Kea and end_allow_threads when called by Kea
extern void begin_allow_threads();
extern void end_allow_threads();

extern void log_error(std::string msg);
extern int Capsule_define();

// cfg_mgr.cc
typedef struct {
    PyObject_HEAD

    isc::dhcp::CfgMgr *mgr;
} CfgMgrObject;

#define CfgMgr_Check(op) (Py_TYPE(op) == &CfgMgrType)
extern PyTypeObject CfgMgrType;
extern int CfgMgr_define();

// cfg_subnets4.cc
typedef struct {
    PyObject_HEAD

    isc::dhcp::CfgSubnets4Ptr ptr;
} CfgSubnets4Object;

#define CfgSubnets4_Check(op) (Py_TYPE(op) == &CfgSubnets4Type)
extern PyTypeObject CfgSubnets4Type;
extern PyObject *CfgSubnets4_from_ptr(isc::dhcp::CfgSubnets4Ptr &ptr);
extern int CfgSubnets4_define();

// config_backend_dhcp4_mgr.cc
typedef struct {
    PyObject_HEAD

    isc::dhcp::ConfigBackendDHCPv4Mgr *mgr;
} ConfigBackendDHCPv4MgrObject;

#define ConfigBackendDHCPv4Mgr_Check(op) (Py_TYPE(op) == &ConfigBackendDHCPv4MgrType)
extern PyTypeObject ConfigBackendDHCPv4MgrType;
extern int ConfigBackendDHCPv4Mgr_define();

// config_backend_pool_dhcp4.cc
typedef boost::shared_ptr<isc::dhcp::ConfigBackendPoolDHCPv4> ConfigBackendPoolDHCPv4Ptr;
typedef struct {
    PyObject_HEAD

    ConfigBackendPoolDHCPv4Ptr ptr;
} ConfigBackendPoolDHCPv4Object;

#define ConfigBackendPoolDHCPv4Object_Check(op) (Py_TYPE(op) == &ConfigBackendPoolDHCPv4Type)
extern PyTypeObject ConfigBackendPoolDHCPv4Type;
extern PyObject *ConfigBackendPoolDHCPv4_from_ptr(ConfigBackendPoolDHCPv4Ptr &ptr);
extern int ConfigBackendPoolDHCPv4_define();

// constants.cc
extern int Constants_define();

// host.cc
typedef struct {
    PyObject_HEAD

    bool is_const;
    isc::dhcp::HostPtr ptr;
    isc::dhcp::ConstHostPtr const_ptr;
} HostObject;

#define Host_Check(op) (Py_TYPE(op) == &HostType)
extern PyTypeObject HostType;
extern PyObject *Host_from_ptr(isc::dhcp::HostPtr host);
extern PyObject *Host_from_constptr(isc::dhcp::ConstHostPtr host);
extern int Host_define();

// host_mgr.cc
typedef struct {
    PyObject_HEAD

    isc::dhcp::HostMgr *mgr;
} HostMgrObject;

#define HostMgr_Check(op) (Py_TYPE(op) == &HostMgrType)
extern PyTypeObject HostMgrType;
extern int HostMgr_define();

// host_reservation_parser4.cc
typedef struct {
    PyObject_HEAD

    isc::dhcp::HostReservationParser4 *parser;
} HostReservationParser4Object;

#define HostReservationParser4_Check(op) (Py_TYPE(op) == &HostReservationParser4Type)
extern PyTypeObject HostReservationParser4Type;
extern int HostReservationParser4_define();

// lease4.cc
typedef struct {
    PyObject_HEAD

    isc::dhcp::Lease4Ptr ptr;
} Lease4Object;

#define Lease4_Check(op) (Py_TYPE(op) == &Lease4Type)
extern PyTypeObject Lease4Type;
extern PyObject *Lease4_from_ptr(isc::dhcp::Lease4Ptr &ptr);
extern int Lease4_define();

// lease_mgr.cc
typedef struct {
    PyObject_HEAD

    isc::dhcp::LeaseMgr *mgr;
} LeaseMgrObject;

#define LeaseMgr_Check(op) (Py_TYPE(op) == &LeaseMgrType)
extern PyTypeObject LeaseMgrType;
extern int LeaseMgr_define();

// library_handle.cc
typedef struct {
    PyObject_HEAD

    isc::hooks::LibraryHandle *handle;
    bool is_owner;
} LibraryHandleObject;

#define LibraryHandle_Check(op) (Py_TYPE(op) == &LibraryHandleType)
extern PyTypeObject LibraryHandleType;
extern PyObject *LibraryHandle_from_handle(isc::hooks::LibraryHandle *handle);
extern int LibraryHandle_define();

// logger_manager.cc
typedef struct {
    PyObject_HEAD
} LoggerManagerObject;
extern PyTypeObject LoggerManagerType;
extern int LoggerManager_define();

// option.cc
typedef struct {
    PyObject_HEAD

    isc::dhcp::OptionPtr ptr;
} OptionObject;

#define Option_Check(op) (Py_TYPE(op) == &OptionType)
extern PyTypeObject OptionType;
extern PyObject *Option_from_ptr(isc::dhcp::OptionPtr &ptr);
extern int Option_define();

// pkt4.cc
typedef struct {
    PyObject_HEAD

    isc::dhcp::Pkt4Ptr ptr;
} Pkt4Object;

#define Pkt4_Check(op) (Py_TYPE(op) == &Pkt4Type)
extern PyTypeObject Pkt4Type;
extern PyObject *Pkt4_from_ptr(isc::dhcp::Pkt4Ptr &ptr);
extern int Pkt4_define();

// server.cc
typedef struct {
    PyObject_HEAD

    isc::db::ServerPtr ptr;
} ServerObject;

#define Server_Check(op) (Py_TYPE(op) == &ServerType)
extern PyTypeObject ServerType;
extern PyObject *Server_from_ptr(isc::db::ServerPtr &ptr);
extern int Server_define();

// srv_config.cc
typedef struct {
    PyObject_HEAD

    isc::dhcp::SrvConfigPtr ptr;
} SrvConfigObject;

#define SrvConfig_Check(op) (Py_TYPE(op) == &SrvConfigType)
extern PyTypeObject SrvConfigType;
extern PyObject *SrvConfig_from_ptr(isc::dhcp::SrvConfigPtr &ptr);
extern int SrvConfig_define();

// subnet4.cc
typedef struct {
    PyObject_HEAD

    isc::dhcp::Subnet4Ptr ptr;
} Subnet4Object;

#define Subnet4_Check(op) (Py_TYPE(op) == &Subnet4Type)
extern PyTypeObject Subnet4Type;
extern PyObject *Subnet4_from_ptr(isc::dhcp::Subnet4Ptr &ptr);
extern int Subnet4_define();

// subnet4_config_parser.cc
typedef struct {
    PyObject_HEAD

    isc::dhcp::Subnet4ConfigParser *parser;
} Subnet4ConfigParserObject;

#define Subnet4ConfigParser_Check(op) (Py_TYPE(op) == &Subnet4ConfigParserType)
extern PyTypeObject Subnet4ConfigParserType;
extern int Subnet4ConfigParser_define();

// utils.cc
extern int assert_long_value(PyObject *value, const char *name);
extern int assert_string_value(PyObject *value, const char *name, bool allow_none);
extern int assert_bool_value(PyObject *value, const char *name);
extern PyObject *element_to_object(isc::data::ConstElementPtr ptr);
extern isc::data::ElementPtr object_to_element(PyObject *obj);

}
