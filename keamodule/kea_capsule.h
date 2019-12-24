#ifndef KEA_CAPSULE_H
#define KEA_CAPSULE_H

extern "C" {

typedef struct {
    void (*debug)(const char *msg);
    void (*info)(const char *msg);
    void (*warn)(const char *msg);
    void (*error)(const char *msg);
    void (*fatal)(const char *msg);
} LogFunctions;

#define Kea_Bootstrap_NUM 0
#define Kea_Bootstrap_RETURN int
#define Kea_Bootstrap_PROTO (LogFunctions *log_functions)

#define Kea_Shutdown_NUM 1
#define Kea_Shutdown_RETURN int
#define Kea_Shutdown_PROTO ()

#define Kea_API_pointers 2


#ifdef KEA_MODULE

static Kea_Bootstrap_RETURN Kea_Bootstrap Kea_Bootstrap_PROTO;
static Kea_Shutdown_RETURN Kea_Shutdown Kea_Shutdown_PROTO;

#else

static void **kea_capsule;

#define Kea_Bootstrap (*(Kea_Bootstrap_RETURN (*)Kea_Bootstrap_PROTO) kea_capsule[Kea_Bootstrap_NUM])
#define Kea_Shutdown (*(Kea_Shutdown_RETURN (*)Kea_Shutdown_PROTO) kea_capsule[Kea_Shutdown_NUM])

#endif

}

#endif /* !defined(KEA_CAPSULE_H) */
