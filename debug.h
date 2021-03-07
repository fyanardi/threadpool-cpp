#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#if __linux
#include <sys/syscall.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

/**
 * Writes the C string pointed by format to the standard output (stdout) prefixed with other
 * information useful in a multi-threaded environment: time and current thread id. The format
 * accepted is the same as the format accepted by the printf function as this function also
 * uses printf to write the final formatted output.
 *
 * Like printf, this is C only and does not require C++.
 *
 * @param format printf style C string that contains the text to be written to the stdout
 */
int debug(const char *format, ...) {
    char buffer[1024];
    char time_buf[9];
    time_t now = time(0);
    strftime (time_buf, 9, "%H:%M:%S", localtime (&now));
#if __linux
    snprintf(buffer, 1024, "[%s] [%ld] %s", time_buf, syscall(__NR_gettid), format);
#elif defined(_WIN32) || defined(_WIN64)
    snprintf(buffer, 1024, "[%s] [%lu] %s", time_buf, GetCurrentThreadId(), format);
#endif

    char buffer1[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer1, 1024, buffer, args);
    va_end(args);
    return printf(buffer1);
}

#endif // DEBUG_H

