#ifndef MWL_SRC_UTIL_H
#define MWL_SRC_UTIL_H

#include <errno.h>
#include <string.h>

/* Util uses abort macro for graceful exit from function
 * MWL_ASSERT is does not throw, simply exits and logs error
 */
#if defined MWL_LOG_ERROR && defined MWL_ABORT
#define MWL_ASSERT(X)\
if (!(X)) {\
        fprintf(stderr, "\n\033[31mMWL_ERROR: %s - line %d: %s\033[0m\n\n",\
                __FILE__, __LINE__, strerror(errno));\
        goto MWL_ABORT;\
    }
#elif defined MWL_LOG_ERROR
#define MWL_ASSERT(X)\
    if (!(X)) {\
        fprintf(stderr, "\n\033[31mMWL_ERROR: %s - line %d: %s\033[0m\n\n",\
                __FILE__, __LINE__, strerror(errno));\
    }
#elif defined MWL_ABORT
#define MWL_ASSERT(X)\
    if (!(X)) {\
        goto MWL_ABORT;\
    }
#else
#define MWL_ASSERT(X) if (!(X)) {}
#endif

// Log the function aborted from
#ifndef __FUNCTION_NAME__
    #ifdef WIN32   //WINDOWS
        #define __FUNCTION_NAME__   __FUNCTION__  
    #else          //*NIX
        #define __FUNCTION_NAME__   __func__ 
    #endif
#endif
#ifdef MWL_LOG_EARLY_EXIT
#define MWL_LOG_ABORT() fprintf(stderr, "\033[31mMWL_ABORT: function - %s early exit\033[0m\n", __FUNCTION_NAME__)
#else
#define MWL_LOG_ABORT()
#endif

// Increase readability of many comparasons
#define MWL_WITHIN(X, A, B) (X > A && X < B)

// Big Endian to Little Endian conversion
unsigned long MWL_ntohl(unsigned long n);

// Little Endian to Big Endian conversion
unsigned long MWL_htonl(unsigned long n);

// Show stack-trace if debug build
void MWL_handle_error(void);

#endif //MWL_SRC_UTIL_H
