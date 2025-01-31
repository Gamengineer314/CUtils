#include "throw.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#ifndef NO_STACKTRACE // execinfo.h is not standard, recompile with -DNO_STACKTRACE in case of error
#include <execinfo.h>
#endif


void stacktrace() {
#ifndef NO_STACKTRACE
    // Get stacktrace
    void* stacktrace[64];
    int n = backtrace(stacktrace, 64);
    char** symbols = backtrace_symbols(stacktrace, n);

    // Print stacktrace
    char command[512];
    char symbol[256];
    for (int i = 1; i < n; i++) {
        // Copy symbol
        snprintf(symbol, 256, "%s", symbols[i]);
        
        // Split program and address
        char* address = symbol;
        while (*address != '(' && *address != 0) address++;
        if (*address == 0) {
            fprintf(stderr, "| %s\n", symbols[i]);
            continue;
        }
        *address = 0;

        // Split address and rest
        address++;
        char* end = address;
        while (*end != ')' && *end != 0) end++;
        if (*end == 0) {
            fprintf(stderr, "| %s\n", symbols[i]);
            continue;
        }
        *end = 0;

        // Print using addr2line
        fprintf(stderr, "| ");
        snprintf(command, 512, "addr2line -C -p -s -f -e %s %s >&2", symbol, address);
        if (system(command)) {
            while (i < n) {
                fprintf(stderr, "| %s\n", symbols[i++]);
            }
            break;
        }
    }

    free(symbols);
#endif
}


void _throw_error(const char* err, ...) {
    va_list args;
    va_start(args, err);
    vfprintf(stderr, err, args);
    va_end(args);
    stacktrace();
    exit(1);
}


void _throw(int code) {
    fprintf(stderr, "Error !\n%s (%d)\n", strerror(code), code);
    stacktrace();
    exit(1);
}


void _throw_msg(int code, const char* msg, ...) {
    fprintf(stderr, "Error !\n");
    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n%s (%d)\n", strerror(code), code);
    stacktrace();
    exit(1);
}


void _throw_c(int code) {
    fprintf(stderr, "Error !\n%d\n", code);
    stacktrace();
    exit(1);
}


void _throw_c_msg(int code, const char* msg, ...) {
    fprintf(stderr, "Error !\n");
    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n%d\n", code);
    stacktrace();
    exit(1);
}