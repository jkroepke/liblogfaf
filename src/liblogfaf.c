/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <pthread.h>

#define MAX_MESSAGE_LEN 65536

#ifdef NDEBUG
#define DBG(x)
#else
#define DBG(x) debugprintf x

static void debugprintf(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fflush(stderr);
}
#endif

__attribute__((constructor)) static void _liblogfaf_init(void) {
    DBG(("liblogfaf: init()\n"));
}

__attribute__((destructor)) static void _liblogfaf_fini(void) {
    DBG(("liblogfaf: fini()\n"));
}

void openlog(const char *ident, int option, int facility) {
    DBG(("liblogfaf: openlog(%s, %d, %d)\n", ident, option, facility));
}

void closelog(void) {
    DBG(("liblogfaf: closelog()\n"));
}

void __syslog_chk(int priority, int flag, const char *format, ...) {
    DBG(("liblogfaf: __syslog_chk(%d, %d, %s)\n",
         priority, flag, format));
    va_list ap;
    char str[MAX_MESSAGE_LEN];
    va_start(ap, format);
    vsnprintf(str, MAX_MESSAGE_LEN, format, ap);
    va_end(ap);
    printf("%s", str);
}

void syslog(int priority, const char *format, ...) {
    DBG(("liblogfaf: syslog(%d, %s)\n", priority, format));
    va_list ap;
    char str[MAX_MESSAGE_LEN];
    va_start(ap, format);
    vsnprintf(str, MAX_MESSAGE_LEN, format, ap);
    va_end(ap);
    printf("%s", str);
}

