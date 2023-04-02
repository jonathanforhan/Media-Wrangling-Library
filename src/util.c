#include "util.h"

#include <execinfo.h>
#include <stdio.h>
#include <malloc.h>

unsigned long MWL_ntohl(unsigned long n) {
    return  ((n >> 24) & 0xff) |      // byte 3 to byte 0
            ((n >> 8)  & 0xff00) |    // byte 2 to byte 1
            ((n << 8)  & 0xff0000) |  // byte 1 to byte 2
            ((n << 24) & 0xff000000); // byte 0 to byte 3
}

void MWL_handle_error(void) {
#if !defined NDEBUG && defined linux
    void* callstack[128];
    int frames = backtrace(callstack, 128);
    char** strs = backtrace_symbols(callstack, frames);
    for (int i = 0; i < frames; ++i) {
        printf("%s\n", strs[i]);
    }
    free(strs);
#endif
}
