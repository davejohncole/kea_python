#ifndef KEA_CAPSULE_H
#define KEA_CAPSULE_H

extern "C" {

#define Kea_SetLogger_NUM 0
#define Kea_SetLogger_RETURN void
#define Kea_SetLogger_PROTO (isc::log::Logger &logger, const isc::log::MessageID& ident)

#define Kea_Bootstrap_NUM 1
#define Kea_Bootstrap_RETURN int
#define Kea_Bootstrap_PROTO (isc::hooks::LibraryHandle *handle, const char *module_name)

#define Kea_Shutdown_NUM 2
#define Kea_Shutdown_RETURN int
#define Kea_Shutdown_PROTO ()

#define Kea_API_pointers 3

#ifdef KEA_MODULE
static Kea_Bootstrap_RETURN Kea_Bootstrap Kea_Bootstrap_PROTO;
static Kea_Shutdown_RETURN Kea_Shutdown Kea_Shutdown_PROTO;
#else
static void **kea_capsule;

#define Kea_SetLogger (*(Kea_SetLogger_RETURN (*)Kea_SetLogger_PROTO) kea_capsule[Kea_SetLogger_NUM])
#define Kea_Bootstrap (*(Kea_Bootstrap_RETURN (*)Kea_Bootstrap_PROTO) kea_capsule[Kea_Bootstrap_NUM])
#define Kea_Shutdown (*(Kea_Shutdown_RETURN (*)Kea_Shutdown_PROTO) kea_capsule[Kea_Shutdown_NUM])
#endif

}

#endif /* !defined(KEA_CAPSULE_H) */
