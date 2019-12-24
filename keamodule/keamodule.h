#ifndef KEAMODULE_H
#define KEAMODULE_H
#ifdef __cplusplus
extern "C" {
#endif

/* C API functions */
#define Kea_System_NUM 0
#define Kea_System_RETURN int
#define Kea_System_PROTO (const char *command)

/* Total number of C API pointers */
#define Kea_API_pointers 1


#ifdef KEA_MODULE
/* This section is used when compiling keamodule.c */

static Kea_System_RETURN Kea_System Kea_System_PROTO;

#else
/* This section is used in modules that use keamodule's API */

static void **Kea_API;

#define Kea_System (*(Kea_System_RETURN (*)Kea_System_PROTO) Kea_API[Kea_System_NUM])

/* Return -1 on error, 0 on success.
 * PyCapsule_Import will set an exception if there's an error.
 */
static int
import_spam(void) {
    Kea_API = (void **)PyCapsule_Import("kea._C_API", 0);
    return (Kea_API != NULL) ? 0 : -1;
}

#endif

#ifdef __cplusplus
}
#endif

#endif /* !defined(KEAMODULE_H) */
