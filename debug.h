#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#if defined (__unix__)
#include <math.h>
#include <sys/syscall.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#define MAX_DEBUG_BUFFER 1024

/**
 * Writes the C string pointed by format to the standard output (stdout) prefixed with other
 * information useful in a multi-threaded environment: time and current thread id. The format
 * accepted is the same as the format accepted by the printf function as this function uses
 * vsnprintf to write the final formatted output. Thread id information is only available on
 * either Unix or Windows environments.
 *
 * Like printf, this is C only and does not require C++.
 *
 * @param format printf style C string that contains the text to be written to the stdout
 */
int debug(const char *format, ...) {
    char buffer[MAX_DEBUG_BUFFER];

#if defined (__unix__)
    char time_buf[9];
    long ms;
    time_t s;
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s = spec.tv_sec;
    ms = spec.tv_nsec / 1.0e6;
    strftime(time_buf, 9, "%H:%M:%S", localtime(&s));
    snprintf(buffer, MAX_DEBUG_BUFFER, "[%s.%03ld] [%ld] %s", time_buf, ms, syscall(__NR_gettid),
            format);
#elif defined(_WIN32) || defined(_WIN64)
    SYSTEMTIME time;
    GetSystemTime(&time);

    char time_buf[13];
    snprintf(time_buf, 13, "%02d:%02d:%02d.%03d", time.wHour, time.wMinute, time.wSecond,
            time.wMilliseconds);
    snprintf(buffer, MAX_DEBUG_BUFFER, "[%s] [%lu] %s", time_buf, GetCurrentThreadId(), format);
#else
    char time_buf[9];
    time_t now = time(0);
    strftime(time_buf, 9, "%H:%M:%S", localtime(&s))
    snprintf(buffer, MAX_DEBUG_BUFFER, "[%s] %s", time_buf, format);
#endif

    char buffer1[MAX_DEBUG_BUFFER];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer1, MAX_DEBUG_BUFFER, buffer, args);
    va_end(args);
    return printf(buffer1);
}

#endif // DEBUG_H

