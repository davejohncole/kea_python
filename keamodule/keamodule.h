#include <ffi.h>
#include <config.h>
#include <hooks/hooks.h>
#include <hooks/callout_manager.h>
#include <log/macros.h>
#include <dhcpsrv/lease.h>
#include <dhcp/pkt4.h>
#include <dhcpsrv/cfgmgr.h>
#include <dhcpsrv/lease_mgr.h>
#include <dhcpsrv/host_mgr.h>
#include <dhcp/dhcp4.h>
#include <dhcpsrv/parsers/host_reservation_parser.h>

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

// capsule.cc
extern PyObject *hook_module;
extern isc::log::Logger *kea_logger;
extern isc::log::MessageID *kea_message_id;
extern void begin_allow_threads();
extern void end_allow_threads();

extern void log_error(std::string msg);
extern int Capsule_define();

// constants.cc
extern int Constants_define();

// utils.cc
extern int assert_long_value(PyObject *value, const char *name);
extern int assert_string_value(PyObject *value, const char *name, bool allow_none);
extern int assert_bool_value(PyObject *value, const char *name);
extern PyObject *element_to_object(isc::data::ConstElementPtr ptr);
extern isc::data::ElementPtr object_to_element(PyObject *obj);

// callout_closure.cc
typedef struct {
    PyObject_HEAD

    PyObject *name;
    PyObject *callout;
    ffi_cif cif;
    ffi_type *args[1];
    void *bound_callout;
    ffi_closure *closure;
} CalloutClosureObject;

extern PyObject *CalloutClosure_from_object(PyObject *name, PyObject *callout);
extern int CalloutClosure_define();

// callouts.cc
extern int Callouts_add_closure(CalloutClosureObject *obj);
extern int Callouts_register(isc::hooks::LibraryHandle *handle);
extern int Callouts_unregister();

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

// callout_manager.cc
typedef struct {
    PyObject_HEAD

    boost::shared_ptr<isc::hooks::CalloutManager> manager;
} CalloutManagerObject;

#define CalloutManager_Check(op) (Py_TYPE(op) == &CalloutManagerType)
extern PyTypeObject CalloutManagerType;
extern int CalloutManager_define();

// callout_handle.cc
typedef struct {
    PyObject_HEAD

    isc::hooks::CalloutHandle *handle;
    bool is_owner;
} CalloutHandleObject;

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

// lease4.cc
typedef struct {
    PyObject_HEAD

    isc::dhcp::Lease4Ptr ptr;
} Lease4Object;

#define Lease4_Check(op) (Py_TYPE(op) == &Lease4Type)
extern PyTypeObject Lease4Type;
extern PyObject *Lease4_from_handle(isc::dhcp::Lease4Ptr &ptr);
extern int Lease4_define();

// pkt4.cc
typedef struct {
    PyObject_HEAD

    isc::dhcp::Pkt4Ptr ptr;
} Pkt4Object;

#define Pkt4_Check(op) (Py_TYPE(op) == &Pkt4Type)
extern PyTypeObject Pkt4Type;
extern PyObject *Pkt4_from_handle(isc::dhcp::Pkt4Ptr &ptr);
extern int Pkt4_define();

// option.cc
typedef struct {
    PyObject_HEAD

    isc::dhcp::OptionPtr ptr;
} OptionObject;

#define Option_Check(op) (Py_TYPE(op) == &OptionType)
extern PyTypeObject OptionType;
extern PyObject *Option_from_handle(isc::dhcp::OptionPtr &ptr);
extern int Option_define();

// cfg_mgr.cc
typedef struct {
    PyObject_HEAD
} CfgMgrObject;

#define CfgMgr_Check(op) (Py_TYPE(op) == &CfgMgrType)
extern PyTypeObject CfgMgrType;
extern int CfgMgr_define();

// srv_config.cc
typedef struct {
    PyObject_HEAD

    isc::dhcp::SrvConfigPtr ptr;
} SrvConfigObject;

#define SrvConfig_Check(op) (Py_TYPE(op) == &SrvConfigType)
extern PyTypeObject SrvConfigType;
extern PyObject *SrvConfig_from_ptr(isc::dhcp::SrvConfigPtr &ptr);
extern int SrvConfig_define();

// cfg_subnets4.cc
typedef struct {
    PyObject_HEAD

    isc::dhcp::CfgSubnets4Ptr ptr;
} CfgSubnets4Object;

#define CfgSubnets4_Check(op) (Py_TYPE(op) == &CfgSubnets4Type)
extern PyTypeObject CfgSubnets4Type;
extern PyObject *CfgSubnets4_from_ptr(isc::dhcp::CfgSubnets4Ptr &ptr);
extern int CfgSubnets4_define();

// lease_mgr.cc
typedef struct {
    PyObject_HEAD

    isc::dhcp::LeaseMgr *mgr;
} LeaseMgrObject;

#define LeaseMgr_Check(op) (Py_TYPE(op) == &LeaseMgrType)
extern PyTypeObject LeaseMgrType;
extern int LeaseMgr_define();

// subnet4.cc
typedef struct {
    PyObject_HEAD

    isc::dhcp::Subnet4Ptr ptr;
} Subnet4Object;

#define Subnet4_Check(op) (Py_TYPE(op) == &Subnet4Type)
extern PyTypeObject Subnet4Type;
extern PyObject *Subnet4_from_ptr(isc::dhcp::Subnet4Ptr &ptr);
extern int Subnet4_define();

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

// host_reservation_parser.cc
typedef struct {
    PyObject_HEAD

    isc::dhcp::HostReservationParser4 *parser;
} HostReservationParserObject;

#define HostReservationParser_Check(op) (Py_TYPE(op) == &HostReservationParserType)
extern PyTypeObject HostReservationParserType;
extern int HostReservationParser_define();

}
