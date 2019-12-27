#ifndef KEA_CAPSULE_H
#define KEA_CAPSULE_H

extern "C" {

#define Kea_SetLogger_NUM 0
#define Kea_SetLogger_RETURN void
#define Kea_SetLogger_PROTO (isc::log::Logger &logger, const isc::log::MessageID& ident)

#define Kea_Load_NUM 1
#define Kea_Load_RETURN int
#define Kea_Load_PROTO (isc::hooks::LibraryHandle *handle, const char *module_name)

#define Kea_Unload_NUM 2
#define Kea_Unload_RETURN int
#define Kea_Unload_PROTO ()

#define Kea_API_pointers 3

#ifdef KEA_MODULE
static Kea_Load_RETURN Kea_Load Kea_Load_PROTO;
static Kea_Unload_RETURN Kea_Unload Kea_Unload_PROTO;
#else
static void **kea_capsule;

#define Kea_SetLogger (*(Kea_SetLogger_RETURN (*)Kea_SetLogger_PROTO) kea_capsule[Kea_SetLogger_NUM])
#define Kea_Load (*(Kea_Load_RETURN (*)Kea_Load_PROTO) kea_capsule[Kea_Load_NUM])
#define Kea_Unload (*(Kea_Unload_RETURN (*)Kea_Unload_PROTO) kea_capsule[Kea_Unload_NUM])
#endif

}

#endif /* !defined(KEA_CAPSULE_H) */
