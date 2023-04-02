#ifndef MWL_SRC_UTIL_H
#define MWL_SRC_UTIL_H

#include <errno.h>
#include <string.h>

/* Util uses abort macro for graceful exit from function
 * MWL_ASSERT is does not throw, simply exits and logs error
 */
#if !defined NDEBUG && defined MWL_ABORT
#define MWL_ASSERT(X)\
    if (!(X)) {\
        const char *red = "\033[31m";\
        const char *none = "\033[0m";\
        fprintf(stderr, "\n%sMWL_ERROR: %s - line %d: %s%s\n\n",\
                red, __FILE__, __LINE__, strerror(errno), none);\
        MWL_handle_error();\
        goto MWL_ABORT;\
    }
#elif !defined NDEBUG
#define MWL_ASSERT(X)\
    if (!(X)) {\
        const char *red = "\033[31m";\
        const char *none = "\033[0m";\
        fprintf(stderr, "\n%sMWL_ERROR: %s - line %d: %s%s\n\n",\
                red, __FILE__, __LINE__, strerror(errno), none);\
        MWL_handle_error();\
    }
#elif defined MWL_ABORT
#define MWL_ASSERT(X)\
    if (!(X)) {\
        goto MWL_ABORT;\
    }
#else
#define MWL_ASSERT(X) X
#endif

// Big Endian to Little Endian conversion
unsigned long MWL_ntohl(unsigned long n);

// Show stack-trace if debug build
void MWL_handle_error(void);

#endif //MWL_SRC_UTIL_H
